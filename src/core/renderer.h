#pragma once
#include "framebuffer.h"
#include "gbuffer.h"
#include "utility.h"
#include "window.h"

namespace FlatEngine {
	class Renderer {
	public:

		static void InitRenderer();
		static void BeginRendering();
		static void EndRendering();

		static void CreateGBuffer();
		static Ref<FBuffer> GetFramebuffer();
		static Ref<GBuffer> GetGbuffer();
		static void ClearColor(float R, float G, float B, float W);
		static void Clear();
	private:
		static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
		static Ref<FBuffer> m_fBuffer;
		static Ref<GBuffer> m_gBuffer;
	};
}
