#pragma once
#include "framebuffer.h"
#include "gbuffer.h"
#include "ssao.h"
#include "utils/utility.h"
#include "render/window.h"
#include "core/core.h"
#include "imgui.h"
#include <core/camera.h>
namespace FlatEngine {
	class Renderer {
	public:
		enum class FrameBufferSizeType{
			Custom,
			Fullscreen
		};
		static ImVec2 frameBufferSize;
		static void InitRenderer();
		static void BeginRendering();
		static void EndRendering();
		static void ClearColor(float R, float G, float B, float W);
		static void Clear();

		static void SetFrameBufferSizeType(FrameBufferSizeType type);
		static void CreateFrameBuffer();
		static void CreateGBuffer();
		static void ResizeBuffers();
		static void CreateSSAOBuffers();
		static Ref<FBuffer> GetFramebuffer();
		static Ref<GBuffer> GetGbuffer();
		static Ref<SSAO> GetSSAOBuffer();
		static void AddCamera(const Ref<Camera>& camera);
		static Ref<Camera> GetCamera(int index);
	private:
		static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
		static Ref<FBuffer> m_fBuffer;
		static Ref<GBuffer> m_gBuffer;
		static Ref<SSAO>	m_SSAOBuffer;
		static std::vector<Ref<Camera>> cameraList;
	};
}
