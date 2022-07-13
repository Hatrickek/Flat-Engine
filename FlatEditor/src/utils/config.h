#pragma once
#include <filesystem>
#include <string>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
namespace FlatEngine {
	class EditorConfig {
	public:
		static void LoadInternalConfig();
		static void SaveInternalConfig();
		static bool LoadConfig();
		static void LoadConfig(std::filesystem::path path);
		static void SaveConfigWithDialog();
		//dataSize = How many lines of value it has, not the actual size.
		static void SaveConfig(std::filesystem::path path);
	};
}
