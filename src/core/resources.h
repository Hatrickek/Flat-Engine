#pragma once
#include "render/model.h"
#include "utils/utility.h"

namespace FlatEngine {
	class Resources {
	public:
		static void InitResources();
		static Ref<Model>  GetDefaultCube();
		static Ref<Shader> GetDefaultShader();
		static Ref<Shader> GetLightingShader();
	};
}
