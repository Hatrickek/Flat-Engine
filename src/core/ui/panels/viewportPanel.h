#pragma once
#include <glm/glm.hpp>
namespace FlatEngine {
	class ViewportPanel {
	public:
		static void DrawViewport();
	private:
		static void DrawPerformanceOverlay();
	};
}