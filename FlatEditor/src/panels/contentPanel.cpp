#include "contentPanel.h"
#include "imgui.h"
#include "utils/filesystem.h"
#include <filesystem>
#include <render/texture.h>
namespace FlatEngine {
	void ContentPanel::Init() {
		//Texture(FileSystem("").c_str(), );
	}
	void ContentPanel::DrawPanel() {
		const std::filesystem::path contentPath = "resources";
		static std::filesystem::path currentDir = std::filesystem::path(contentPath);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoFocusOnAppearing;
		ImGui::Begin("Assets", NULL, window_flags);

		if(currentDir != contentPath) {
			if(ImGui::Button("<")) {
				currentDir = currentDir.parent_path();
			}
		}
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));
		for(auto& p : std::filesystem::directory_iterator(currentDir)) {
			const auto& path = p.path();
			auto relativePath = std::filesystem::relative(path, contentPath);
			std::string fileName = relativePath.filename().string();
			if(p.is_directory()) {
				if(ImGui::Button(fileName.c_str())) {
					currentDir /= p.path().filename();
				}
			}
			else {
				if(ImGui::Button(fileName.c_str())) {
				}
			}
		}
		ImGui::PopStyleColor();

		ImGui::End();
	};
}