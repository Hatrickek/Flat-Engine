#include "input.h"
#include "application.h"
#include "utils/timestep.h"
#include "render/renderer.h"
namespace FlatEngine {
	void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void ProcessInput(GLFWwindow* window);
	float mouseOffsetX;
	float mouseOffsetY;
	float mouseScrollOffsetY;
	void Input::Init() {
		glfwSetCursorPosCallback(Window::GetOpenGLWindow(), MouseCallback);
		glfwSetScrollCallback(Window::GetOpenGLWindow(), ScrollCallback);
	}
	void Input::Update() {
		ProcessInput(Window::GetOpenGLWindow());
	}
	bool Input::GetKey(KeyCode key) {
		GLFWwindow* window = Window::GetOpenGLWindow();
		auto state = glfwGetKey(window, key);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	//TODO: Right now this function is the same as GetKey().
	bool Input::GetKeyDown(KeyCode key) {
		GLFWwindow* window = Window::GetOpenGLWindow();
		auto state = glfwGetKey(window, key);
		return state == GLFW_PRESS;
	}
	bool Input::GetKeyUp(KeyCode key) {
		GLFWwindow* window = Window::GetOpenGLWindow();
		auto state = glfwGetKey(window, key);
		return state == GLFW_RELEASE;
	}

	bool Input::GetMouseButtonDown(const MouseCode button) {
		GLFWwindow* window = Window::GetOpenGLWindow();
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	glm::vec2 Input::GetMousePosition() {
		double xpos, ypos;
		glfwGetCursorPos(Window::GetOpenGLWindow(), &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}
	float Input::GetMouseOffsetX(){
		return mouseOffsetX;
	}
	float Input::GetMouseOffsetY(){
		return mouseOffsetY;
	}
	float Input::GetMouseScrollOffsetY(){
		return mouseScrollOffsetY;
	}
	void Input::SetCursorPosition(float X, float Y) {
		GLFWwindow* window = Window::GetOpenGLWindow();
		glfwSetCursorPos(window, X, Y);
	}
	Input::CursorState cursor_state = Input::CursorState::DISABLED;
	Input::CursorState Input::GetCursorState() {
		return cursor_state;
	}
	void Input::SetCursorState(CursorState state, GLFWwindow* window) {
		switch(state) {
		case Input::CursorState::DISABLED:
			cursor_state = Input::CursorState::DISABLED;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		case Input::CursorState::NORMAL:
			cursor_state = Input::CursorState::NORMAL;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		case Input::CursorState::HIDDEN:
			cursor_state = Input::CursorState::HIDDEN;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;
		}
	}
	float lastX = (float)Window::SCR_WIDTH / 2.0;
	float lastY = (float)Window::SCR_HEIGHT / 2.0;
	bool firstMouse = true;
	void MouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);
		if(firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		mouseOffsetX = xpos - lastX;
		mouseOffsetY = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		Renderer::GetCamera(0)->ProcessMouseMovement(mouseOffsetX, mouseOffsetY);

	}
	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		mouseScrollOffsetY = yoffset;
	}
	void ProcessInput(GLFWwindow* window) {
		if(glfwGetKey(window, GLFW_KEY_F12) == GLFW_PRESS)
			Application::Get().Close();
	}
}
