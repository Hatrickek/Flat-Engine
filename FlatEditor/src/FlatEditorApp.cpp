#include <FlatEngine.h>
#include <core/EntryPoint.h>
#include "editorlayer.h"

namespace FlatEngine {
	class FlatEditor : public Application {
	public:
		FlatEditor(const AppSpec& spec) : Application(spec) {
			PushLayer(new EditorLayer());
		}
	};
	Application* CreateApplication(ApplicationCommandLineArgs args) {
		AppSpec spec;
		spec.Name = "FlatEngine - Editor";
		spec.WorkingDirectory = std::filesystem::current_path().string();
		spec.CommandLineArgs = args;
		return new FlatEditor(spec);
	}
}