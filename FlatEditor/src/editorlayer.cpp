#include "editorlayer.h"
#include "panels/sceneHPanel.h"
#include "utils/uiutils.h"
#include "panels/viewportPanel.h"
#include "panels/consolePanel.h"
#include "panels/contentPanel.h"
#include "utils/config.h"
#include "panels/panelSettings.h"
#include "core/resources.h"
#include <filesystem>
#include <algorithm>
namespace FlatEngine {
	EditorLayer* EditorLayer::m_Instance = nullptr;
	EditorLayer::EditorLayer() : Layer("Editor Layer") {
		m_Instance = this;
	}
	void EditorLayer::OnAttach() {
		Renderer::SetFrameBufferSizeType(Renderer::FrameBufferSizeType::Custom);
		Renderer::InitRenderer();
		Renderer::CreateFrameBuffer();
		Renderer::CreateGBuffer();
		Renderer::CreateSSAOBuffers();
		
		EditorConfig::LoadInternalConfig();
		Ref<Camera> m_EditorCamera = CreateRef<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
		m_EditorCamera->SetPosition(glm::vec3(1, 2, 5));
		Renderer::AddCamera(m_EditorCamera);
		Resources::InitResources();

		Renderer::GetSSAOBuffer()->SetupSSAOShader(Resources::GetSSAOShader(), Resources::GetSSAOBlurShader());
		Input::SetCursorState(Input::CursorState::NORMAL, Window::GetOpenGLWindow());

		//test code.
		//Model rsr_model(FileSystem::getPath("resources/objects/911rsr.obj"));
		//Model patrick_model(FileSystem::getPath("resources/objects/patrick_animated/patrick_animated.obj"));

		m_ActiveScene = CreateRef<Scene>();

		Entity cube = m_ActiveScene->CreateEntity("Cube");
		cube.AddComponent<PrimitiveRendererComponent>().primitive = CUBE;
		cube.GetComponent<TransformComponent>().Scale = glm::vec3(10, 0.2, 10);

		//Entity rsr = m_ActiveScene->CreateEntity("RsrModel");
		//rsr.GetComponent<TransformComponent>().Scale = glm::vec3(1.5f);
		//rsr.GetComponent<TransformComponent>().Translation = glm::vec3(-1.0f, 0.2, -2.0);
		//rsr.AddComponent<MeshRendererComponent>(CreateRef<Model>(rsr_model), Resources::GetDefaultShader(), WHITE);

		//Entity patrick = m_ActiveScene->CreateEntity("PatrickModel");
		//patrick.GetComponent<TransformComponent>().Scale = glm::vec3(.3f);
		//patrick.GetComponent<TransformComponent>().Translation = glm::vec3(2.5f, 0.1, -3.0);
		//patrick.GetComponent<TransformComponent>().Rotation = glm::vec3(0, 180, 0);
		//patrick.AddComponent<MeshRendererComponent>(CreateRef<Model>(patrick_model), Resources::GetDefaultShader(), BLANK);

		Entity pointLight = m_ActiveScene->CreateEntity("Point Light");
		pointLight.AddComponent<LightComponent>().color = glm::vec4(0.5f);
		pointLight.GetComponent<TransformComponent>().Translation = glm::vec3(0.0f, 6.0f, 0.0f);

	}
	void EditorLayer::OnDetach() {
		EditorConfig::SaveInternalConfig();
		glfwTerminate();
	}
	void EditorLayer::OnUpdate(float deltaTime) {
		if(ViewportPanel::viewportFocused)
			Renderer::GetCamera(0)->OnUpdate();
		Renderer::ClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		Renderer::Clear();
		
		Renderer::GetGbuffer()->Begin();
		glm::mat4 projection = Renderer::GetCamera(0)->GetProjectionMatrix();
		glm::mat4 view = Renderer::GetCamera(0)->GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		Resources::GetSSAOGeometryShader()->use();
		Resources::GetSSAOGeometryShader()->setMat4("projection", projection);
		Resources::GetSSAOGeometryShader()->setMat4("view", view);
		if(m_ActiveScene)
			m_ActiveScene->OnUpdate(Timestep::GetDeltaTime());
		
		Renderer::GetGbuffer()->End();
		
		Renderer::GetSSAOBuffer()->BeginSSAOTexture(projection, Renderer::GetGbuffer()->gPosition, Renderer::GetGbuffer()->gNormal);
		Draw::RenderQuad();
		Renderer::GetSSAOBuffer()->EndSSAOTexture();

		Renderer::GetSSAOBuffer()->BeginSSAOBlurTexture();
		Draw::RenderQuad();
		Renderer::GetSSAOBuffer()->EndSSAOBlurTexture();
		
		Lighting::UpdateLighting();
		
		Renderer::BeginRendering();
		Draw::RenderQuad();
		Renderer::EndRendering();
		
		if(Input::GetKeyDown(Key::F)){
			Ref<Scene> newScene = CreateRef<Scene>(); 
			m_ActiveScene = newScene;
		}
		
	}
	void EditorLayer::OnImGuiRender() {
		EditorShortcuts();
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", NULL, window_flags);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("Main dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		if(ImGui::BeginMenuBar()) {
			if(ImGui::BeginMenu("File")) {
				if(ImGui::MenuItem("New Scene", "Ctrl + N")) {
					NewScene();
				}
				if(ImGui::MenuItem("Open Scene", "Ctrl + O")) {
					OpenScene();
				}
				if(ImGui::BeginMenu("Open Recent")) {
					//for(auto& scene : PanelSettings::lastOpenedScenes) {
					//	if(!scene.empty()) {
					//		//TODO:FIX
					//		if(ImGui::MenuItem(scene.c_str())) {
					//			if(!OpenScene(scene)) {
					//				scene.erase();
					//			}
					//		}
					//	}
					//}
					ImGui::EndMenu();
				}

				if(ImGui::MenuItem("Save Scene", "Ctrl + S")) {
					SaveScene();
				}
				if(ImGui::MenuItem("Save Scene As...", "Ctrl + Shift + S")) {
					SaveSceneAs();
				}
				ImGui::Separator();
				if(ImGui::MenuItem("Exit")) {
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("Edit")) {
				if(ImGui::MenuItem("Settings")) {
					PanelSettings::settingsMenuOn = true;
				}
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("Window")) {
				ImGui::MenuItem("Inspector", 0, &PanelSettings::inspector_window);
				ImGui::MenuItem("Scene hierarchy", 0, &SceneHPanel::scenehierarchy_window);
				ImGui::MenuItem("Console window", 0, &PanelSettings::console_window);
				ImGui::MenuItem("Performance Overlay", 0, &PanelSettings::performance_overlay);
				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("About");
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if(ImGui::BeginMenuBar()) {
			ImGui::SetCursorPos(ImVec2(Window::SCR_WIDTH * 0.5f, 0));
			if(ImGui::MenuItem(">>")) {
				EditorLayer::SetEditorState(EditorState::Play);
			}
			if(ImGui::MenuItem("||")) {
				EditorLayer::SetEditorState(EditorState::Edit);

			}
			ImGui::EndMenuBar();
		}

		ViewportPanel::DrawPanel();
		SceneHPanel::DrawPanel();
		ConsolePanel::DrawPanel();
		DrawDebugPanel();
		SettingsPanel();
		//ContentPanel::DrawPanel(); //@INCOMPLETE

		ImGui::End();
	}
	void EditorLayer::EditorShortcuts() {
		if(Input::GetKey(Key::LeftControl)) {
			if(Input::GetKeyDown(Key::N)) {
				EditorLayer::NewScene();
			}
			if(Input::GetKeyDown(Key::S)) {
				EditorLayer::SaveScene();
			}
			if(Input::GetKeyDown(Key::O)) {
				EditorLayer::OpenScene();
			}
			if(Input::GetKey(Key::LeftShift) && Input::GetKeyDown(Key::S)) {
				EditorLayer::SaveSceneAs();
			}
		}
		
	}
	void EditorLayer::NewScene() {
		Ref<Scene> newScene = CreateRef<Scene>();
		EditorLayer::SetActiveScene(newScene);
	}
	void EditorLayer::OpenScene() {
		std::string filepath = FileDialogs::OpenFile("FlatEngine Scene (*.scene)\0*.scene\0");
		if(!filepath.empty())
			OpenScene(filepath);
	}
	bool EditorLayer::OpenScene(const std::filesystem::path& path) {
		if(!std::filesystem::exists(path)) {
			FE_LOG_WARN("Could not find {0}", path.filename().string());
			return false;
		}
		if(path.extension().string() != ".scene") {
			FE_LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return false;
		}
		if(PanelSettings::lastOpenedScenes.size() > 8) {
			PanelSettings::lastOpenedScenes.erase(PanelSettings::lastOpenedScenes.begin());
		}
		if(std::find(PanelSettings::lastOpenedScenes.begin()
			, PanelSettings::lastOpenedScenes.end(), path.string()) != PanelSettings::lastOpenedScenes.end()) {
			PanelSettings::lastOpenedScenes.emplace_back(path.string());
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if(serializer.Deserialize(path.string())) {
			EditorLayer::SetActiveScene(newScene);
		}
		return true;
	}
	void EditorLayer::SaveScene() {
		if(!PanelSettings::lastSaveScenePath.empty()) {
			SceneSerializer serializer(EditorLayer::GetActiveScene());
			serializer.Serialize(PanelSettings::lastSaveScenePath);
		}
		else {
			EditorLayer::SaveSceneAs();
		}
	}
	void EditorLayer::SaveSceneAs() {
		std::string filepath = FileDialogs::SaveFile("FlatEngine Scene (*.scene)\0*.scene\0");
		if(!filepath.empty()) {
			SceneSerializer serializer(EditorLayer::GetActiveScene());
			serializer.Serialize(filepath);
		}
	}
	void EditorLayer::SettingsPanel() {
		if(!PanelSettings::settingsMenuOn) return;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		ImGui::Begin("Settings", &PanelSettings::settingsMenuOn, window_flags);
		{//Theme
			const char* themes[] = { "Dark", "White" };
			if(ImGui::Combo("Theme", &Application::Get().GetImGuiLayer()->selectedTheme, themes, FE_ARRAYSIZE(themes))) {
				Application::Get().GetImGuiLayer()->SetTheme(Application::Get().GetImGuiLayer()->selectedTheme);
			}
		}
		if(ImGui::Button("Load Config")) {
			EditorConfig::LoadConfig();
		}
		ImGui::SameLine();
		if(ImGui::Button("Save Config")) {
			EditorConfig::SaveConfigWithDialog();
		}
		ImGui::End();
	}
	void EditorLayer::DrawDebugPanel() {
		ImGui::Begin("DebugInfo");
		ImGui::Text("Lighting");
		ImGui::SliderFloat("Ambient", &Lighting::ambientLight, 0, 1, "%.2f");
		ImGui::Separator();
		ImGui::Text("SSAO");
		ImGui::SameLine();
		static bool ssao_toggle = true;
		if(ImGui::Checkbox("Toggle", &ssao_toggle)) {
			SSAO::ssao_radius = ssao_toggle ? 0.5f : 0.0f;
		}

		static int kernelSize = 64;
		ImGui::SliderInt("Kernel size", &kernelSize, 16, 256, "%d");
		SSAO::SetKernelSize(kernelSize);

		static float radius = 0.5f;
		ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f, "%.2f");
		if(ssao_toggle) SSAO::SetRadius(radius);

		static float sample = 64;
		ImGui::SliderFloat("Samples", &sample, 16, 256, "%.0f");

		ImGui::End();
	}
	Ref<Scene> EditorLayer::GetActiveScene() {
		return m_ActiveScene;
	}
	void EditorLayer::SetActiveScene(Ref<Scene> scene) {
		m_ActiveScene = scene;
	}
	void EditorLayer::SetEditorState(EditorState state) {
		m_EditorState = state;

		switch(m_EditorState) {
		case EditorState::Edit:
			Input::SetCursorState(Input::CursorState::NORMAL, Window::GetOpenGLWindow());
			break;
		case EditorState::Play:
			Input::SetCursorState(Input::CursorState::DISABLED, Window::GetOpenGLWindow());
			Input::SetCursorPosition(Window::GetWindowSize().x * 0.5f, Window::GetWindowSize().y * 0.5f);
			break;
		}

	}
}