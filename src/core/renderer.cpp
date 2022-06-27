#include "renderer.h"
#include "window.h"
namespace FlatEngine {
	Ref<FBuffer> Renderer::m_fBuffer;

	void Renderer::InitRenderer() {
		m_fBuffer = CreateRef<FBuffer>(Window::SCR_WIDTH, Window::SCR_HEIGHT);
	}
	
	void Renderer::BeginRendering() {
		m_fBuffer->Begin();
	}
	void Renderer::EndRendering() {
		m_fBuffer->End();
	}
	Ref<FBuffer> Renderer::GetFramebuffer() {
		return m_fBuffer;
	}
	void Renderer::ClearColor(float R, float G, float B, float W) {
		glClearColor(R, G, B, W);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}
