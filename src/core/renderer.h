#pragma once
#include "framebuffer.h"
#include "utility.h"
namespace FlatEngine {
	class Renderer {
	public:

		static void InitRenderer();
		static void BeginRendering();
		static void EndRendering();
		
		static Ref<FBuffer> GetFramebuffer();
		static void ClearColor(float R, float G, float B, float W);
	private:
		static Ref<FBuffer> m_fBuffer;
	};
}