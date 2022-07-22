#pragma once
#include "entt/entt.hpp"

namespace FlatEngine {

	class Entity;

	class Scene {
	public:
		Scene();
		Scene(const std::string& name);
		~Scene();
		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);
		Entity DuplicateEntity(Entity entity);
		void OnUpdate(float deltaTime);
		Entity FindEntity(const std::string& name);

		entt::registry m_Registry;
		std::string sceneName = "Untitled";
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHPanel;
	};
}