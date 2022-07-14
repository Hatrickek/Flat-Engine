#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <glm/gtx/quaternion.hpp>
#include <utility>

#include "render/model.h"
#include "utils/utility.h"

namespace FlatEngine {
#define WHITE glm::vec4(1,1,1,1)
#define BLANK glm::vec4(0,0,0,0)
#define RED   glm::vec4(1,0,0,1)
#define GREEN glm::vec4(0,1,0,1)
#define BLUE  glm::vec4(0,0,1,1)

	enum Primitive {
		NONE,
		CUBE,
		QUAD
	};
	enum LightType {
		POINT,
		DIRECTIONAL
	};
	enum RigidBodyType {
		STATIC,
		DYNAMIC
	};
	struct TagComponent {
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {
		}
	};

	struct TransformComponent {
		glm::vec3 Translation = glm::vec3(0);
		glm::vec3 Rotation = glm::vec3(0);
		glm::vec3 Scale = glm::vec3(1);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {
		}

		glm::mat4 GetTransform() const {
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};
	struct MeshRendererComponent {
		Ref<Model> model = nullptr;
		glm::mat4 m_model = glm::mat4(1.0f);
		Ref<Shader> shader = nullptr;
		std::vector<glm::vec4> colors;
		MeshRendererComponent() = default;
		MeshRendererComponent(Ref<Model> model, Ref<Shader> shader, glm::vec4 diffuseColor = WHITE)
			: model(std::move(model)), shader(std::move(shader)){
			 colors[0] = (diffuseColor); 
		}
	};
	struct PrimitiveRendererComponent {
		Primitive primitive = CUBE;
		Ref<Shader> shader;
		glm::mat4 m_model = glm::mat4(1.0f);
		glm::vec4 color = WHITE;
		PrimitiveRendererComponent() = default;
		PrimitiveRendererComponent(Primitive primitive, Ref<Shader> shader, glm::vec4 color)
			: primitive(primitive), shader(std::move(shader)), color(color) {
		}
	};
	struct LightComponent {
		bool enabled = true;
		LightType type = POINT;
		glm::vec4 color = WHITE;
		LightComponent() = default;
	};
	struct RigidBodyComponent {
		RigidBodyType type = STATIC;
		RigidBodyComponent() = default;
	};
	struct BoxColliderComponent {
		glm::vec3 size = glm::vec3(1);
		BoxColliderComponent() = default;
	};

	class ScriptableEntity;
	
	struct NativeScriptComponent {
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind() {
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	template<typename... Component>
	struct ComponentGroup {
	};

	using AllComponents =
		ComponentGroup<TransformComponent, LightComponent, PrimitiveRendererComponent, MeshRendererComponent,
		RigidBodyComponent, BoxColliderComponent>;

}
