#pragma once
#include <glm/glm.hpp>

#include "imgui.h"

namespace FlatEngine {
	class ViewportPanel {
	public:
		static glm::vec2 m_ViewportBounds[2];
		static ImVec2 viewportPanelSize;
		static void DrawViewport();
	private:
		static void DrawPerformanceOverlay();
	};
}
