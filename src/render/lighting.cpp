#include "lighting.h"

#include "renderer.h"
#include "core/resources.h"

namespace FlatEngine {
	float Lighting::ambientLight = 0.4f;
	void Lighting::UpdateLighting() {
		Renderer::Clear();
		Resources::GetLightingShader()->use();
		// Update attenuation parameters
		Resources::GetLightingShader()->setFloat("ambientLight", ambientLight);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Renderer::GetGbuffer()->gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Renderer::GetGbuffer()->gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Renderer::GetGbuffer()->gAlbedo);
		glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
		glBindTexture(GL_TEXTURE_2D, Renderer::GetSSAOBuffer()->ssaoColorBufferBlur);
	}
}