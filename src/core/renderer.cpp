#include "renderer.h"

#include "ui/panels/viewportPanel.h"

namespace FlatEngine {
	Ref<FBuffer> Renderer::m_fBuffer;
	Ref<GBuffer> Renderer::m_gBuffer;
	void Renderer::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
		GetFramebuffer()->RegenerateBuffers(ViewportPanel::viewportPanelSize.x, ViewportPanel::viewportPanelSize.y);
		m_gBuffer->RegenerateBuffers(width, height);
		//m_ssao->RegenerateBuffers(width, height);
		Window::SCR_HEIGHT = height;
		Window::SCR_WIDTH = width;

		glViewport(0, 0, width, height);
	}
	void Renderer::InitRenderer() {
		glfwSetFramebufferSizeCallback(Window::GetOpenGLWindow(), FrameBufferSizeCallback);
		m_fBuffer = CreateRef<FBuffer>(Window::SCR_WIDTH, Window::SCR_HEIGHT);
	}
	
	void Renderer::BeginRendering() {
		m_fBuffer->Begin();
	}
	void Renderer::EndRendering() {
		m_fBuffer->End();
	}
	void Renderer::CreateGBuffer() {
		CreateRef<GBuffer>(ViewportPanel::viewportPanelSize.x, ViewportPanel::viewportPanelSize.y);
	}
	Ref<GBuffer> Renderer::GetGbuffer() {
		return m_gBuffer;
	}
	Ref<FBuffer> Renderer::GetFramebuffer() {
		return m_fBuffer;
	}
	void Renderer::ClearColor(float R, float G, float B, float W) {
		glClearColor(R, G, B, W);
	}
	void Renderer::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}
