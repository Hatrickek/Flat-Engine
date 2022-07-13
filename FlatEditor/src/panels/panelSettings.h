#pragma once
#include <vector>
#include <string>
namespace FlatEngine {
	class PanelSettings {
	public:
		static bool console_window;
		static bool inspector_window;
		static bool performance_overlay;
		static bool settingsMenuOn;
		static std::string lastSaveScenePath;
		static std::vector<std::string> lastOpenedScenes;
	};
}