#pragma once
#include "FlatEngine.h"
#include "core/camera.h"

namespace FlatEngine {
	class EditorLayer : public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(float deltaTime) override;
		virtual void OnImGuiRender() override;
		static EditorLayer& Get(){ return *m_Instance; }

		void EditorShortcuts();
		Ref<Scene> GetActiveScene();
		void SetActiveScene(Ref<Scene> scene);
		enum class EditorState{
			Edit = 0, Play = 1
		};
		void SetEditorState(EditorState state);
		EditorState m_EditorState = EditorState::Edit;
	private:
		void NewScene();
		void OpenScene();
		bool OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
		void SettingsPanel();
		void DrawDebugPanel();
		Ref<Scene> m_ActiveScene;
		static EditorLayer* m_Instance;
		
	};
}