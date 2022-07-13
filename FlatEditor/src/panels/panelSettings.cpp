#include "panelSettings.h"
namespace FlatEngine {
	bool PanelSettings::console_window = true;
	bool PanelSettings::inspector_window = true;
	bool PanelSettings::performance_overlay = true;
	bool PanelSettings::settingsMenuOn = false;
	std::string PanelSettings::lastSaveScenePath;
	std::vector<std::string> PanelSettings::lastOpenedScenes;
}