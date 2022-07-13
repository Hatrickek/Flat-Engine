#pragma once
#include "render/model.h"
#include "utils/utility.h"

namespace FlatEngine {
	class Resources {
	public:
		static void InitResources();
		static void LoadShaders();
		static Ref<Model>  GetDefaultCube();
		static Ref<Shader> GetDefaultShader();
		static void SetDefaultShader(Ref<Shader> shader);
		static Ref<Shader> GetLightingShader();
		static void SetLightingShader(Ref<Shader> shader);
		static Ref<Shader> GetSSAOGeometryShader();
		static void SetSSAOGeometryShader(Ref<Shader> shader);
		static Ref<Shader> GetSSAOShader();
		static void SetSSAOShader(Ref<Shader> shader);
		static Ref<Shader> GetSSAOBlurShader();
		static void SetSSAOBlurShader(Ref<Shader> shader);
	};
}
