#include "glm/glm.hpp"
#include "scene.h"

#include "render/draw.h"
#include "entity.h"
#include "resources.h"
#include "render/renderer.h"
#include "scriptableEntity.h"

namespace FlatEngine {

	Scene::Scene() = default;
	Scene::Scene(const std::string& name) : sceneName(name) {

	}
	Scene::~Scene() = default;

	Entity Scene::CreateEntity(const std::string& name) {
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		FE_LOG_INFO("Created an entity: {}", tag.Tag);
		return entity;
	}
	void Scene::DestroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}
	Entity Scene::DuplicateEntity(Entity entity) {//TODO: A better way to do this. @INCOMPLETE
		const std::string name = fmt::format("{} Copy", entity.GetComponent<TagComponent>().Tag);
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
	Entity Scene::FindEntity(const std::string& name) {
		const auto group = m_Registry.view<TagComponent>();
		for(const auto entity : group) {
			auto& tag = group.get<TagComponent>(entity);
			if(tag.Tag == name) {
				return Entity{entity, this};
			}
		}
		return {};
	}
	void Scene::OnUpdate(float deltaTime) {
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
				// TODO: Move to Scene::OnScenePlay
				if(!nsc.Instance) {
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(deltaTime);
				});
		}
		{
			const auto group = m_Registry.view<TransformComponent, MeshRendererComponent>();
			for(const auto entity : group) {
				auto [transform, meshrenderer] = group.get<TransformComponent, MeshRendererComponent>(entity);
				Draw::DrawModel(meshrenderer, transform.Translation, transform.Scale, transform.Rotation);
			}
		}
		{
			const auto group = m_Registry.view<TransformComponent, PrimitiveRendererComponent>();
			for(const auto entity : group) {
				auto [transform, prc] = group.get<TransformComponent, PrimitiveRendererComponent>(entity);
				Draw::DrawPrimitive(prc, transform.Translation, transform.Scale, transform.Rotation);
			}
		}
		{
			const auto group = m_Registry.view<TransformComponent, LightComponent>();
			static int i = 0; //@TEMP
			for(const auto entity : group) {
				auto [transform, light] = group.get<TransformComponent, LightComponent>(entity);
				const float linear = 0.09f;
				const float quadratic = 0.032f;
				Resources::GetLightingShader()->use();
				glm::vec3 lightPosView = glm::vec3(Renderer::GetCamera(0)->GetViewMatrix() * glm::vec4(transform.Translation, 1.0));
				Resources::GetLightingShader()->setVec3(std::format("light[{}].Position", i), lightPosView);
				Resources::GetLightingShader()->setVec3(std::format("light[{}].Color", i), light.color);
				Resources::GetLightingShader()->setFloat(std::format("light[{}].Linear", i), linear);
				Resources::GetLightingShader()->setFloat(std::format("light[{}].Quadratic", i), quadratic);
				//@TEMP
				if(i > group.size_hint()) {
					i = 0;
				}
				else {
					i++;
				}
			}
		}
	}
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component) {
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {
	}
	template<>
	void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component) {
		if(!component.model) {
			component.model = Resources::GetDefaultCube();
		}
		if(!component.shader) {
			component.shader = Resources::GetDefaultShader();
		}
		component.colors = component.model->colors;
	}
	template<>
	void Scene::OnComponentAdded<PrimitiveRendererComponent>(Entity entity, PrimitiveRendererComponent& component) {
		component.shader = Resources::GetDefaultShader();
		component.primitive = CUBE;
	}
	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component) {
	}
	template<>
	void Scene::OnComponentAdded<BoxColliderComponent>(Entity entity, BoxColliderComponent& component) {
	}
	template<>
	void Scene::OnComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component) {
	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {

	}
}
