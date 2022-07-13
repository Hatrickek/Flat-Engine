#pragma once
#include "core/scene.h"
#include "core/components.h"
#include "core/entity.h"
namespace FlatEngine {
	class SceneHPanel {
	public:
		static bool scenehierarchy_window;

		static void DrawPanel();
		static void DrawInspectorPanel();
		static void DrawEntityNode(Entity entity);
		static Entity GetSelectedEntity();
	private:
		static void DrawComponents(Entity entity);
		static Entity m_SelectionContext;
	};
}
