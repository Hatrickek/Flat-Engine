#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "core/input.h"
#include "core/core.h"
namespace FlatEngine {
	class Window {
	public:
		static int SCR_WIDTH;
		static int SCR_HEIGHT;

		static void InitOpenGLWindow();
		static void UpdateWindow();
		static void CloseWindow(GLFWwindow* window);
		static GLFWwindow* GetOpenGLWindow();
		static glm::vec2 GetWindowSize();
		
	private:
		static GLFWwindow* window;
	};
}