#pragma once
#include "core/camera.h"
#include "core/core.h"
namespace FlatEngine {
	class Lighting {
	public:
		static float ambientLight;
		static void UpdateLighting();
	};
}
