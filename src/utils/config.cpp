#include "config.h"
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

#include "log.h"
#include "ui/ui.h"
#include "ui/uiutils.h"
namespace FlatEngine{
    void Config::LoadInternalConfig(){
        const std::filesystem::path path = "feinternal.feconfig";
        if(exists(path)){
            YAML::Node data;
        	try {
	    		data = YAML::LoadFile(path.string());
	    	}
	    	catch (YAML::ParserException& e) {
	    		return;
	    	}
            auto lssp = data["LastSaveScenePath"];
            if(!lssp.IsNull()) {
            	UI::lastSaveScenePath = lssp.as<std::string>();
            }
            auto losp = data["LastOpenedScenes"];
            if(!losp.IsNull() ) {
                for(auto scene : losp){
	               UI::lastOpenedScenes.emplace_back(scene.as<std::string>());
                }
            }
    		FE_LOG_INFO("Loaded config: '{}'", path.filename().string());
        }
        else{
            SaveInternalConfig();
        }
    }
    void Config::SaveInternalConfig(){
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "LastSaveScenePath" << YAML::Value << UI::lastSaveScenePath;
        
        out << YAML::Key << "LastOpenedScenes" << YAML::Value << YAML::BeginSeq;
        for(auto scene : UI::lastOpenedScenes){
            out << YAML::BeginMap;
            out << YAML::Key << YAML::Value << scene;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        
        out << YAML::EndMap;
        std::ofstream fout("feinternal.feconfig");
		fout << out.c_str();
    }
    bool Config::LoadConfig(){
        std::string path = FileDialogs::OpenFile("FE Config (*.feconfig)\0*.feconfig\0");
        if(!path.empty()) {
            LoadConfig(path);
            return true;
        }
        return false;
    }
    void Config::LoadConfig(std::filesystem::path path){
        if (path.extension().string() != ".feconfig")
		{
			FE_LOG_WARN("Could not load {0} - not a Flat Engine config file", path.filename().string());
			return;
		}
        YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException& e) {
			return;
		}
        UI::selectedTheme = data["Theme"].as<int>();

		FE_LOG_INFO("Loaded config: '{}'", path.filename().string());
    }
    void Config::SaveConfig(){
        std::string filepath = FileDialogs::SaveFile("FE Config (*.feconfig)\0*.feconfig\0");
		if (!filepath.empty())
		{
			 YAML::Emitter out;
             out << YAML::BeginMap;

			out << YAML::Key << "Theme" << YAML::Value << UI::selectedTheme;

			out << YAML::EndMap;
             std::ofstream fout(filepath);
		     fout << out.c_str();
		}
    }
}