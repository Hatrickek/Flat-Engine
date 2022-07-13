#pragma once
#include "core/scene.h"
#include "core/components.h"
#include "core/entity.h"
namespace FlatEngine {
	class SceneHPanel {
	public:
		static bool scenehierarchy_window;
		static bool renameEntity;

		static void DrawPanel();
		static void DrawInspectorPanel();
		static void DrawEntityNode(Entity entity);
		static Entity GetSelectedEntity();
	private:
		static Entity m_SelectionContext;
		static void DrawComponents(Entity entity);
	};
}
