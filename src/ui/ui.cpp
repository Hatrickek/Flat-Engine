#include "ui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo/ImGuizmo.h"

#include <ShObjIdl.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/core.h"
#include "core/editor.h"
#include "core/entity.h"
#include "level.h" //TODO: this is temporary import for accesing the lighting properties.
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "render/ssao.h"
#include "utils/utility.h"
#include "render/window.h"

#include "panels/sceneHPanel.h"
#include "panels/viewportPanel.h"
#include "panels/consolePanel.h"

#include "utils/feMath.h"
#include "uiutils.h"
#include "core/sceneSerializer.h"
#include "utils/filesystem.h"
namespace FlatEngine {
	void UI::InitUI(GLFWwindow* window) {
		// Setup Dear ImGui context
		auto glsl_version = "#version 410";
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		
		ImGuiDarkTheme();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			//style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		io.Fonts->AddFontFromFileTTF(FileSystem::getPath("resources/fonts/Roboto-Medium.ttf").c_str(), 16.0f);
		io.Fonts->AddFontDefault();
		io.Fonts->Build();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		SceneHPanel::SetScene(Editor::GetActiveScene());
	}

	void UI::UpdateImgui() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void UI::RenderImgui() {
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void UI::EndImgui() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	//--------------------------------------
	//Custom Functions

	static bool performance_overlay = true;
	static bool inspector_window = true;
	static bool console_window = true;
	static int m_GizmoType = -1;
	static bool settingsMenuOn = false;
	void UI::DrawEditorUI() {
		ShowImguiDockSpace();
		ViewportPanel::DrawViewport();
		DrawGizmos();
		if (Core::GetEngineState() == Core::EDITING) {
			DrawDebugPanel();
			SceneHPanel::DrawPanel();
			ConsolePanel::DrawConsole();
			SettingsMenu();
		}
	}

	void UI::DrawGizmos() {
		ImGuizmo::BeginFrame();
		Entity selectedEntity = SceneHPanel::GetSelectedEntity();
		if (!selectedEntity && m_GizmoType == -1) return;
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetRect(ViewportPanel::m_ViewportBounds[0].x, ViewportPanel::m_ViewportBounds[0].y, ViewportPanel::m_ViewportBounds[1].x - ViewportPanel::m_ViewportBounds[0].x, ViewportPanel::m_ViewportBounds[1].y - ViewportPanel::m_ViewportBounds[0].y);

		glm::mat4 cameraProjection = Editor::GetEditorCamera()->GetProjectionMatrix();
		glm::mat4 cameraView = Editor::GetEditorCamera()->GetViewMatrix();
		// Entity transform
		glm::mat4 transform;
		TransformComponent tc;
		if(selectedEntity) {
			tc = selectedEntity.GetComponent<TransformComponent>();
			transform = tc.GetTransform();
		}

		// Snapping
		bool snap = Input::GetKey(Key::LeftControl);
		float snapValue = 0.5f; // Snap to 0.5m for translation/scale
		// Snap to 45 degrees for rotation
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = 45.0f;
		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
		                     (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
		                     nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);

			glm::vec3 deltaRotation = rotation - tc.Rotation;
			tc.Translation = translation;
			tc.Rotation += deltaRotation;
			tc.Scale = scale;
		}
		if(Input::GetKeyDown(Key::Q)) 
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = -1;
		}
		if(Input::GetKeyDown(Key::W))
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		}
		if(Input::GetKeyDown(Key::E))
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		}
		if(Input::GetKeyDown(Key::R))
		{
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
		}
	}
	void UI::DrawDebugPanel() {
		ImGui::Begin("DebugInfo");
		ImGui::Text("SSAO");
		ImGui::SameLine();
		static bool ssao_toggle = true;
		if (ImGui::Checkbox("Toggle", &ssao_toggle)) {
			SSAO::ssao_radius = ssao_toggle ? 0.5 : 0;
		}

		static int kernelSize = 64;
		ImGui::SliderInt("Kernel size", &kernelSize, 16, 256, "%d");
		SSAO::SetKernelSize(kernelSize);

		static float radius = 0.5f;
		ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f, "%.2f");
		if (ssao_toggle) SSAO::SetRadius(radius);

		static float sample = 64;
		ImGui::SliderFloat("Samples", &sample, 16, 256, "%.0f");

		ImGui::Separator();

		ImGui::Text("Lighting");
		ImGui::PushItemWidth(80);
		ImGui::Text("Position");
		ImGui::SliderFloat(":x", &lightPositions[0].x, -10, 10, "%.2f");
		ImGui::SameLine();
		ImGui::SliderFloat(":y", &lightPositions[0].y, -10, 10, "%.2f");
		ImGui::SameLine();
		ImGui::SliderFloat(":z", &lightPositions[0].z, -10, 10, "%.2f");
		ImGui::Text("Color");
		ImGui::SliderFloat(":r", &lightColors[0].x, 0, 1, "%.2f");
		ImGui::SameLine();
		ImGui::SliderFloat(":g", &lightColors[0].y, 0, 1, "%.2f");
		ImGui::SameLine();
		ImGui::SliderFloat(":b", &lightColors[0].z, 0, 1, "%.2f");
		ImGui::PopItemWidth();
		ImGui::End();
	}
	void UI::ShowImguiDockSpace() {
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoBackground;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", NULL, window_flags);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		float height = ImGui::GetFrameHeight();

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("Main dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		if (ImGui::BeginViewportSideBar("##TopMenuBar", viewport, ImGuiDir_Up, height, window_flags)) {
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Open Scene")) {
						OpenScene();
					}
					if (ImGui::MenuItem("Save Scene")) {
						FE_LOG_WARN("NOT IMPLEMENTED");
						//SaveScene();
					}
					if(ImGui::MenuItem("Save Scene As...")){
						SaveSceneAs();
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Exit")) {
						Window::ExitWindow(Window::GetOpenGLWindow());
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit")) {
					
					if (ImGui::MenuItem("Settings")) {
						settingsMenuOn = true;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Window")) {
					ImGui::MenuItem("Inspector", 0, &inspector_window);
					ImGui::MenuItem("Scene hierarchy", 0, &SceneHPanel::scenehierarchy_window);
					ImGui::MenuItem("Console window", 0, &console_window);
					ImGui::MenuItem("Performance Overlay", 0, &performance_overlay);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Help")) {
					ImGui::MenuItem("About");
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
		if (ImGui::BeginViewportSideBar("##SecondMenuBar", viewport, ImGuiDir_Up, height, window_flags)) {
			if (ImGui::BeginMenuBar()) {
				ImGui::SetCursorPos(ImVec2(Window::SCR_WIDTH / 2, 0));
				if (ImGui::MenuItem(">>")) {
					Core::SetEngineState(Core::PLAYING);
					Input::SetCursorState(Input::CURSOR_STATE_DISABLED, Window::GetOpenGLWindow());
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("||")) {
					Core::SetEngineState(Core::EDITING);
					Input::SetCursorState(Input::CURSOR_STATE_NORMAL, Window::GetOpenGLWindow());
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
		ImGui::End();
	}
	void UI::SettingsMenu() {
		if(!settingsMenuOn) return;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		ImGui::Begin("Settings", &settingsMenuOn, window_flags);
		{//Theme
			static int m_CurrentTheme = 0;
			const char* themes[] = {"Dark", "White"}; 
			if(ImGui::Combo("Theme", &m_CurrentTheme, themes, FE_ARRAYSIZE(themes)))
				switch(m_CurrentTheme) {
				case 0:
					ImGuiDarkTheme();
					break;
				case 1:
					ImGui::StyleColorsLight();
					break;
				default:
					ImGuiDarkTheme();
					break;
				}
		}
		ImGui::End();
	}
	void UI::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("FlatEngine Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}
	void UI::OpenScene(const std::filesystem::path& path)
	{
		if (path.extension().string() != ".scene")
		{
			FE_LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}
		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			Editor::SetActiveScene(newScene);
			SceneHPanel::SetScene(Editor::GetActiveScene());
		}
	}
	void UI::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("FlatEngine Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(Editor::GetActiveScene());
			serializer.Serialize(filepath);
		}
	}
	
	void UI::ImGuiDarkTheme() {
		ImGuiStyle* style = &ImGui::GetStyle();
	    ImVec4* colors = style->Colors;
		style->WindowBorderSize = 0;
		style->WindowRounding = 1.0f;
		style->FrameRounding = 1.0f;
	    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	    colors[ImGuiCol_Tab]					= ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
	    colors[ImGuiCol_TabActive]				= ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
	    colors[ImGuiCol_TabUnfocused]			= ImVec4(0.7f, 0.180f, 0.000f, 1.000f);
	    colors[ImGuiCol_TabUnfocusedActive]		= ImVec4(0.7f, 0.180f, 0.000f, 1.000f);
	    colors[ImGuiCol_TabHovered]				= ImVec4(0.5f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_Header]					= ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_WindowBg]				= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_PopupBg]				= ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_Border]					= ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		colors[ImGuiCol_BorderShadow]			= ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg]				= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive]			= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg]				= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_TitleBgActive]			= ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg]				= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark]				= ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrab]				= ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button]					= ImVec4(0.824f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_ButtonHovered]			= ImVec4(1.000f, 0.114f, 0.114f, 1.000f);
		colors[ImGuiCol_ButtonActive]			= ImVec4(0.431f, 0.000f, 0.000f, 1.000f);
		colors[ImGuiCol_Header]					= ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_HeaderHovered]			= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive]			= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ResizeGrip]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_PlotLines]				= ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered]		= ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram]			= ImVec4(1.000f, 0.271f, 0.000f, 0.631f);
		colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0, 0, 0, 0.73f);
		colors[ImGuiCol_NavWindowingDimBg]		= ImVec4(0, 0, 0, 0.73f);
	}
	void UI::ImGuiWhiteTheme() {
		ImGuiStyle* style = &ImGui::GetStyle();
	    ImVec4* colors = style->Colors;
		style->WindowBorderSize = 0;
		style->WindowRounding = 3.0f;
		style->FrameRounding = 3.0f;
	    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
	    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
	    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	    colors[ImGuiCol_TitleBg]                = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_SliderGrab]             = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
	    colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	    colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
	    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.35f, 0.35f, 0.35f, 0.17f);
	    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.90f);
	    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
	    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
	    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
		//colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
	    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
	    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.57f, 0.57f, 0.64f, 1.00f);   // Prefer using Alpha=1.0 here
	    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.68f, 0.68f, 0.74f, 1.00f);   // Prefer using Alpha=1.0 here
	    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
	    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	    colors[ImGuiCol_NavHighlight]           = colors[ImGuiCol_HeaderHovered];
	    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	}
}
