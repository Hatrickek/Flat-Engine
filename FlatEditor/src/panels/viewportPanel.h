#pragma once
#include <glm/glm.hpp>

#include "imgui.h"

namespace FlatEngine {
	class ViewportPanel {
	public:
		static ImVec2 m_ViewportBounds[2];
		static ImVec2 viewportPanelSize;
		static void DrawPanel();
		static void DrawGizmo();
		static bool viewportFocused;
		static bool viewportHovered;
	private:
		static void DrawPerformanceOverlay();
	};
}
