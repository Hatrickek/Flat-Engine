#pragma once
#include "utils/timestep.h"
#include "utils/log.h"
#include "core/input.h"
#include "core/core.h"
#include <stdio.h>
#include "layerstack.h"
#include "ui/imguilayer.h"
#include <filesystem>
int main(int argc, char** argv);

namespace FlatEngine {
	struct ApplicationCommandLineArgs {
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const {
			//FE_ASSERT(index < Count);
			return Args[index];
		}
	};
	struct AppSpec {
		std::string Name = "FlatEngine App";
		std::filesystem::path WorkingDirectory = std::filesystem::current_path();
		bool isRunning;
		ApplicationCommandLineArgs CommandLineArgs;
	};
	class Application {
	public:

		Application(const AppSpec& spec);
		virtual ~Application();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		static Application& Get() { return *m_Instance; }
		AppSpec m_Spec;
	private:
		void Run();
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
	private:
		bool m_IsRunning = true;
		static Application* m_Instance;
		friend int ::main(int argc, char** argv);
	};
	Application* CreateApplication(ApplicationCommandLineArgs args);
}