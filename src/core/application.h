#pragma once
#include "utils/timestep.h"
#include "utils/log.h"
namespace FlatEngine {
	class App {
	public:
		static void Init() {
		}
		static void Update() {
			glfwPollEvents();
			Timestep::UpdateTime();
		}
	};
}