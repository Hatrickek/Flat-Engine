#pragma once
#include <filesystem>
#include <string>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
namespace FlatEngine {
	class Config {
	public:
		static YAML::Node LoadConfig(std::filesystem::path path);
		static void SaveConfigWithDialog();
		static void SaveConfig(std::filesystem::path path);
	};
}
