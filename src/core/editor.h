#pragma once
#include "camera.h"
#include "scene.h"

namespace FlatEngine {
	class Editor {
	public:
		static void OnInit();
		static void OnUpdate();
		static Ref<Scene> GetActiveScene();
		static void SetActiveScene(Ref<Scene> scene);
		static Ref<Camera> GetEditorCamera();
	private:
		static Ref<Camera> m_EditorCamera;
		static std::shared_ptr<Scene> m_ActiveScene;
	};
}
