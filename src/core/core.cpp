#include "core.h"
#include <core/input.h>

#include "log.h"
#include "renderer.h"
#include "window.h"

namespace FlatEngine {
	Core::EngineState engine_state;

	void Core::Init() {
		Log::Init();
		Window::InitOpenGLWindow();
		Renderer::InitRenderer();
	}

	void Core::SetEngineState(EngineState state) {
		if(state == PLAYING) {
			Input::SetCursorPosition(Window::GetWindowSize().x/2,Window::GetWindowSize().y/2);
		}
		engine_state = state;
	}
	Core::EngineState Core::GetEngineState() {
		return engine_state;
	}	
}
