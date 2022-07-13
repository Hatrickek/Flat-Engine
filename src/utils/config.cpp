#include "config.h"
#include <fstream>
#include "utils/log.h"
#include "utils/uiutils.h"
#include "core/application.h"
namespace FlatEngine {
	YAML::Node Config::LoadConfig(std::filesystem::path path) {
		YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		}
		catch(YAML::ParserException& e) {
			FE_LOG_WARN("Config load error: {}", e.msg);
		}
		FE_LOG_INFO("Loaded config: '{}'", path.filename().string());
		return data;
	}
	void Config::SaveConfigWithDialog() {
		
	}
	void Config::SaveConfig(std::filesystem::path path) {
	}
}