#include "lighting.h"

#include "level.h"
#include "renderer.h"
#include "core/editor.h"
#include "core/resources.h"

namespace FlatEngine {
	void Lighting::UpdateLighting() {
		Renderer::Clear();
		Resources::GetLightingShader()->use();
		// Update attenuation parameters
		const float linear = 0.09f;
		const float quadratic = 0.032f;
		Resources::GetLightingShader()->setFloat("light.Linear", linear);
		Resources::GetLightingShader()->setFloat("light.Quadratic", quadratic);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetGbuffer()->gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetGbuffer()->gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetGbuffer()->gAlbedo);
		glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetSSAOBuffer()->ssaoColorBufferBlur);
	}
}
