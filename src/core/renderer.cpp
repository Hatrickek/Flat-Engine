#include "renderer.h"

#include "ui/panels/viewportPanel.h"

namespace FlatEngine {
	Ref<FBuffer> Renderer::m_fBuffer;
	Ref<GBuffer> Renderer::m_gBuffer;
	Ref<SSAO> Renderer::m_SSAOBuffer;
	void Renderer::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
		GetFramebuffer()->RegenerateBuffers(ViewportPanel::viewportPanelSize.x, ViewportPanel::viewportPanelSize.y);
		m_gBuffer->RegenerateBuffers(ViewportPanel::viewportPanelSize.x, ViewportPanel::viewportPanelSize.y);
		m_SSAOBuffer->RegenerateBuffers(ViewportPanel::viewportPanelSize.x, ViewportPanel::viewportPanelSize.y);
		Window::SCR_HEIGHT = height;
		Window::SCR_WIDTH = width;

		glViewport(0, 0, ViewportPanel::viewportPanelSize.x, ViewportPanel::viewportPanelSize.y);
	}
	void Renderer::InitRenderer() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE); 
		glfwSetFramebufferSizeCallback(Window::GetOpenGLWindow(), FrameBufferSizeCallback);
		m_fBuffer = CreateRef<FBuffer>(Window::SCR_WIDTH, Window::SCR_HEIGHT);
		CreateGBuffer();
		CreateSSAOBuffers();
	}
	
	void Renderer::BeginRendering() {
		m_fBuffer->Begin();
	}
	void Renderer::EndRendering() {
		m_fBuffer->End();
	}
	void Renderer::CreateGBuffer() {
		m_gBuffer = CreateRef<GBuffer>(Window::SCR_WIDTH, Window::SCR_HEIGHT);
	}
	void Renderer::CreateSSAOBuffers() {
		m_SSAOBuffer = CreateRef<SSAO>(Window::SCR_WIDTH, Window::SCR_HEIGHT);
	}
	Ref<SSAO> Renderer::GetSSAOBuffer() {
		return m_SSAOBuffer;
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
