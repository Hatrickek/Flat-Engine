#include "renderer.h"
#include <vector>
namespace FlatEngine {
	Ref<FBuffer> Renderer::m_fBuffer;
	Ref<GBuffer> Renderer::m_gBuffer;
	Ref<SSAO>	 Renderer::m_SSAOBuffer;
	ImVec2 Renderer::frameBufferSize;
	static Renderer::FrameBufferSizeType framebufferSizeType;
	std::vector<Ref<Camera>> Renderer::cameraList;
	void Renderer::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
		bool isMinimized = glfwGetWindowAttrib(Window::GetOpenGLWindow(), GLFW_ICONIFIED);
		if(!isMinimized){
			Window::SCR_HEIGHT = height;
			Window::SCR_WIDTH  = width; 
		}
		if(framebufferSizeType == FrameBufferSizeType::Fullscreen){
			frameBufferSize.x = Window::SCR_WIDTH;
			frameBufferSize.y = Window::SCR_HEIGHT;
		}
		Renderer::ResizeBuffers();
	}
	void Renderer::ResizeBuffers() {
		//TODO: @INCOMPLETE MAKE THIS MORE GENERIC AND FLEXIBLE.	
		//Maybe hold all the buffers in a vector and iterate.
		// all buffers should be more generic, they should bind manually etc.
		if(m_fBuffer != nullptr) GetFramebuffer()->RegenerateBuffers(frameBufferSize.x, frameBufferSize.y);
		if(m_gBuffer != nullptr) m_gBuffer->RegenerateBuffers(frameBufferSize.x, frameBufferSize.y);
		if(m_SSAOBuffer != nullptr) m_SSAOBuffer->RegenerateBuffers(frameBufferSize.x, frameBufferSize.y);
		glViewport(0, 0, frameBufferSize.x, frameBufferSize.y);
	}
	void Renderer::InitRenderer() {
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_MULTISAMPLE);
		glfwSetFramebufferSizeCallback(Window::GetOpenGLWindow(), FrameBufferSizeCallback);
		
	}
	void Renderer::SetFrameBufferSizeType(FrameBufferSizeType type){
		framebufferSizeType = type;
	}
	void Renderer::BeginRendering() {
		m_fBuffer->Begin();
	}
	void Renderer::EndRendering() {
		m_fBuffer->End();
	}
	void Renderer::CreateFrameBuffer(){
		m_fBuffer = CreateRef<FBuffer>(Window::SCR_WIDTH, Window::SCR_HEIGHT);
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
	void Renderer::AddCamera(const Ref<Camera>& camera) {
		cameraList.emplace_back(camera);
	}
	Ref<Camera> Renderer::GetCamera(int index) {
		return cameraList[index];
	}
}