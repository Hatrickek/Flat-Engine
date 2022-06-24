#include "glm/glm.hpp"
#include "scene.h"
#include "draw.h"
#include "entity.h"
#include "resources.h"

namespace FlatEngine {
	Scene::Scene() = default;
	Scene::~Scene() = default;

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		FE_LOG_INFO("Created an entity: {}", tag.Tag);
		return entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}
	Entity Scene::DuplicateEntity(Entity entity){
		const std::string name = std::format("{} Copy", entity.GetComponent<TagComponent>().Tag);
		Entity newEntity = CreateEntity(name);
		newEntity.GetComponent<TransformComponent>() = entity.GetComponent<TransformComponent>();
		if(entity.HasComponent<MeshRendererComponent>()) {
			newEntity.AddComponent<MeshRendererComponent>();
			newEntity.GetComponent<MeshRendererComponent>() = entity.GetComponent<MeshRendererComponent>();
		}
		if(entity.HasComponent<PrimitiveRendererComponent>()) {
			newEntity.AddComponent<PrimitiveRendererComponent>();
			newEntity.GetComponent<PrimitiveRendererComponent>() = entity.GetComponent<PrimitiveRendererComponent>();
		}
		return newEntity;
	}
	void Scene::OnUpdate(float deltaTime) {
		{
			const auto group = m_Registry.group<TransformComponent>(entt::get<MeshRendererComponent>);
			for(const auto entity : group) {
				auto [transform, meshrenderer] = group.get<TransformComponent, MeshRendererComponent>(entity);
				Draw::DrawModel(meshrenderer, transform.Translation, transform.Scale, transform.Rotation);
			}
		}
		{
			const auto group = m_Registry.view<TransformComponent, PrimitiveRendererComponent>();
			for(auto entity : group) {
				auto [transform, prc] = group.get<TransformComponent, PrimitiveRendererComponent>(entity);
				Draw::DrawPrimitive(prc, transform.Translation, transform.Scale, transform.Rotation);
			}
		}
	}
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component)
	{
		if(!component.model) {component.model = Resources::GetDefaultCube();}			
		if(!component.shader) {
			component.shader = Resources::GetDefaultShader();
			component.color = WHITE;
		}
	}
	template<>
	void Scene::OnComponentAdded<PrimitiveRendererComponent>(Entity entity, PrimitiveRendererComponent& component)
	{
		component.shader = Resources::GetDefaultShader();
		component.primitive = CUBE;
	}
}
