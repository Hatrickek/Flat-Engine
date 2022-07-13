#include "window.h"

#include "utils/log.h"
#include "core/application.h"
namespace FlatEngine {
	int Window::SCR_WIDTH = 1600;
	int Window::SCR_HEIGHT = 900;
	GLFWwindow* Window::window;
	void Window::InitOpenGLWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		// glfw window creation
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, Application::Get().m_Spec.Name.c_str(), NULL, NULL);
		if (window == NULL) {
			FE_LOG_ERROR("Failed to create GLFW window");
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		glfwSetWindowCloseCallback(window, CloseWindow);
		Input::SetCursorState(Input::CursorState::DISABLED, window);

		// glad: load all OpenGL function pointers
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			FE_LOG_ERROR("Failed to initialize GLAD");
			glfwTerminate();
		}
	}
	void Window::UpdateWindow() {
		glfwPollEvents();
		glfwSwapBuffers(GetOpenGLWindow());
	}
	void Window::CloseWindow(GLFWwindow* window)
	{
		Application::Get().Close();
	}
	GLFWwindow* Window::GetOpenGLWindow() {
		return window;
	}
	glm::vec2 Window::GetWindowSize() {
		glm::vec2 size;
		size.x = static_cast<float>(SCR_WIDTH);
		size.y = static_cast<float>(SCR_HEIGHT);
		return size;
	}
}
