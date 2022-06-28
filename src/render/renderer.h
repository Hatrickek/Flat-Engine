#pragma once
#include "framebuffer.h"
#include "gbuffer.h"
#include "ssao.h"
#include "utils/utility.h"
#include "render/window.h"

namespace FlatEngine {
	class Renderer {
	public:

		static void InitRenderer();
		static void BeginRendering();
		static void EndRendering();

		static void ClearColor(float R, float G, float B, float W);
		static void Clear();

		static void CreateGBuffer();
		static void ResizeBuffers();
		static void CreateSSAOBuffers();
		static Ref<FBuffer> GetFramebuffer();
		static Ref<GBuffer> GetGbuffer();
		static Ref<SSAO> GetSSAOBuffer();
	private:
		static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
		static Ref<FBuffer> m_fBuffer;
		static Ref<GBuffer> m_gBuffer;
		static Ref<SSAO>	m_SSAOBuffer;
	};
}
