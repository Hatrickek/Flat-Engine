#include "application.h"
#include "core.h"
#include "render/renderer.h"
#include "render/draw.h"
#include "render/lighting.h"
#include "layer.h"
#include "utils/filesystem.h"

namespace FlatEngine {
	Application* Application::m_Instance = nullptr;

	Application::Application(const AppSpec& spec) : m_Spec(spec) {
		if(m_Instance) {
			FE_LOG_ERROR("Application already exists!");
		}
		else{
			FileSystem::SetRootPath(spec.WorkingDirectory);
			m_Instance = this;
			Core::Init();
			m_ImGuiLayer = new ImGuiLayer();
			PushOverlay(m_ImGuiLayer);
		}
	}
	Application::~Application() {
		Application::Close();
	}
	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void Application::PushOverlay(Layer* layer) {
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	void Application::Run() {
		while(m_IsRunning) {
			Timestep::UpdateTime();
			{
				for(Layer* layer : m_LayerStack)
					layer->OnUpdate(Timestep::GetDeltaTime());
			}

			m_ImGuiLayer->Begin();
			{
				for(Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			Window::UpdateWindow();
		}
	}
	void Application::Close() {
		m_IsRunning = false;
	}
}