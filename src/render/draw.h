#pragma once
#include <glm/glm.hpp>

#include "shader.h"
#include "core/components.h"
#include "core/core.h"
namespace FlatEngine {
	class Draw {
	public:
		static void DrawModel(const MeshRendererComponent& mrc,glm::vec3 position, glm::vec3 scale,glm::vec3 rotation);
		static void DrawCube(Shader& shader,glm::vec4 color,glm::vec3 position, glm::vec3 scale, glm::vec3 rotation = glm::vec3(0));
		static void DrawQuad(Shader& shader, glm::vec4 color, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
		static void DrawPrimitive(const PrimitiveRendererComponent& prc, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
		static void RenderQuad();
	private:
		static void RenderCube();
	};
}
