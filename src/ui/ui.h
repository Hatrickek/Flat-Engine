#pragma once
#include <filesystem>
#include <GLFW/glfw3.h>

namespace FlatEngine{
	class UI {
	public:
		//imgui calls
		static void InitUI(GLFWwindow* window);
		static void RenderImgui();
		static void UpdateImgui();
		static void EndImgui();

		//Custom windows
		static void DrawEditorUI();
		static void ShowImguiDockSpace();
		static void DrawDebugPanel();
		static void DrawGizmos();

		static bool console_window;
		static bool inspector_window;
		static bool performance_overlay;

	private:
		static void OpenScene();
		static void OpenScene(const std::filesystem::path& path);
		static void SaveScene();
		static void SaveSceneAs();
		static void SettingsMenu();
		static void EditorShortcuts();
		static void ImGuiDarkTheme();
		static void ImGuiWhiteTheme();
	};
}
