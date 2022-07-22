#include "config.h"
#include <fstream>
#include "panels/panelSettings.h"
#include "utils/log.h"
#include "utils/uiutils.h"
#include "core/application.h"
namespace FlatEngine {
	void EditorConfig::LoadInternalConfig() {
		const std::filesystem::path path = "feinternal.feconfig";
		if(exists(path)) {
			YAML::Node data;
			try {
				data = YAML::LoadFile(path.string());
			}
			catch(YAML::ParserException& e) {
				return;
			}
			auto losp = data["LastOpenedScenes"];
			if(!losp.IsNull()) {
				PanelSettings::lastOpenedScenes = losp.as<std::vector<std::string>>();
			}
			FE_LOG_INFO("Loaded config: '{}'", path.filename().string());
		}
		else {
			SaveInternalConfig();
		}
	}
	void EditorConfig::SaveInternalConfig() {
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "LastOpenedScenes" << YAML::Value << YAML::BeginSeq;
		for(auto scene : PanelSettings::lastOpenedScenes) {
			out << YAML::BeginMap;
			out << YAML::Key << YAML::Value << scene;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
		std::ofstream fout("feinternal.feconfig");
		fout << out.c_str();
	}
	bool EditorConfig::LoadConfig() {
		std::string path = FileDialogs::OpenFile("FE Config (*.feconfig)\0*.feconfig\0");
		if(!path.empty()) {
			LoadConfig(path);
			return true;
		}
		return false;
	}
	void EditorConfig::LoadConfig(std::filesystem::path path) {
		if(path.extension().string() != ".feconfig") {
			FE_LOG_WARN("Could not load {0} - not a Flat Engine config file", path.filename().string());
			return;
		}
		YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		}
		catch(YAML::ParserException& e) {
			return;
		}
		int selectedTheme = data["Theme"].as<int>();
		Application::Get().GetImGuiLayer()->SetTheme(selectedTheme);
		FE_LOG_INFO("Loaded config: '{}'", path.filename().string());
	}
	void EditorConfig::SaveConfigWithDialog() {
		std::string filepath = FileDialogs::SaveFile("FE Config (*.feconfig)\0*.feconfig\0");
		if(!filepath.empty()) {
			YAML::Emitter out;
			out << YAML::BeginMap;

			out << YAML::Key << "Theme" << YAML::Value << Application::Get().GetImGuiLayer()->selectedTheme;

			out << YAML::EndMap;
			std::ofstream fout(filepath);
			fout << out.c_str();
		}
	}
	void EditorConfig::SaveConfig(std::filesystem::path path) {

	}
}