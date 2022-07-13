#include "imguilayer.h"
#include "render/window.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "utils/filesystem.h"
#include "imguizmo/ImGuizmo.h"
#include "render/renderer.h"
namespace FlatEngine {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer") {
	}

	void ImGuiLayer::OnAttach() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;
		ImGuiDarkTheme();

		io.Fonts->AddFontFromFileTTF(FileSystem::getPath("resources/fonts/Roboto-Medium.ttf").c_str(), 18.0f);
		io.Fonts->AddFontDefault();
		io.Fonts->Build();

		ImGui_ImplGlfw_InitForOpenGL(Window::GetOpenGLWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End() {
		ImGuiIO& io = ImGui::GetIO();

		//io.DisplaySize = ImVec2(Renderer::frameBufferSize.x, Renderer::frameBufferSize.y);
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
	void ImGuiLayer::SetTheme(int index) {
		selectedTheme = index;
		switch(index) {
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
	void ImGuiLayer::ImGuiDarkTheme() {
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		style->WindowBorderSize = 0;
		style->WindowRounding = 1.0f;
		style->FrameRounding = 1.0f;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_TabActive] = ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.7f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.7f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.5f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_Header] = ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.8f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.824f, 0.180f, 0.000f, 1.000f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 0.114f, 0.114f, 1.000f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.431f, 0.000f, 0.000f, 1.000f);
		colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.000f, 0.271f, 0.000f, 0.631f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.73f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0, 0, 0, 0.73f);

		selectedTheme = 0;
	}
}