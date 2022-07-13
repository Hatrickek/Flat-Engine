#include "core.h"
#include <core/input.h>

#include "utils/log.h"
#include "render/renderer.h"
#include "render/window.h"
#include "core/input.h"
namespace FlatEngine {
	Core::EngineState engine_state;

	void Core::Init() {
		Window::InitOpenGLWindow();
		Input::Init();
	}

	void Core::SetEngineState(EngineState state) {
		engine_state = state;
	}
	Core::EngineState Core::GetEngineState() {
		return engine_state;
	}	
}
