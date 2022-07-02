#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/core.h"

#include "utils/filesystem.h"
#include "render/shader.h"
#include "core/camera.h"
#include "render/draw.h"
#include "ui/ui.h"
#include "core/input.h"
//#include "render/animation/animation.h"
//#include "render/animation/animator.h"
#include "render/window.h"
#include "utils/log.h"

#include "render/ssao.h"
#include "level.h"
#include "core/application.h"
#include "core/editor.h"
#include "render/gbuffer.h"
#include "core/scene.h"
#include "render/lighting.h"

#include "render/renderer.h"
#include "ui/panels/viewportPanel.h"

//**TODO: Move to input handling 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);
//**

// camera
float lastX = (float)FlatEngine::Window::SCR_WIDTH / 2.0;
float lastY = (float)FlatEngine::Window::SCR_HEIGHT / 2.0;
bool firstMouse = true;

int main() {
	// glfw: initialize and configure
	FlatEngine::Core::Init();

	//**TODO: Move to input handling
	glfwSetCursorPosCallback(FlatEngine::Window::GetOpenGLWindow(), mouse_callback);
	glfwSetScrollCallback(FlatEngine::Window::GetOpenGLWindow(), scroll_callback);
	//**
	
	FlatEngine::Editor::OnInit();

	// build and compile shaders
	FlatEngine::Shader shader_ssao_geometry_pass(FileSystem::getPath("resources/shaders/ssao_geometry.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_geometry.fs").c_str());
	FlatEngine::Shader shader_ssao(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao.fs").c_str());
	FlatEngine::Shader shader_ssao_blur(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_blur.fs").c_str());
	FlatEngine::Renderer::GetSSAOBuffer()->SetupSSAOShader(&shader_ssao, &shader_ssao_blur);
	
	// render loop
	// -----------
	FlatEngine::Editor::GetEditorCamera()->SetPosition(glm::vec3(1,1,3));

	while (!glfwWindowShouldClose(FlatEngine::Window::GetOpenGLWindow())) {

		FlatEngine::App::Update();

		ProcessInput(FlatEngine::Window::GetOpenGLWindow());

		FlatEngine::UI::UpdateImgui();
		
		FlatEngine::Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		FlatEngine::Renderer::Clear();

		FlatEngine::Renderer::GetGbuffer()->Begin();

		glm::mat4 projection = FlatEngine::Editor::GetEditorCamera()->GetProjectionMatrix();
		glm::mat4 view = FlatEngine::Editor::GetEditorCamera()->GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shader_ssao_geometry_pass.use();
		shader_ssao_geometry_pass.setMat4("projection", projection);
		shader_ssao_geometry_pass.setMat4("view", view);

		shader_ssao_geometry_pass.setInt("invertedNormals", 1);
		FlatEngine::Draw::DrawCube(shader_ssao_geometry_pass, WHITE,glm::vec3(0.0, 7.0f, 0.0f), glm::vec3(7.5f, 7.5f, 7.5f));
		shader_ssao_geometry_pass.setInt("invertedNormals", 0);

		FlatEngine::Editor::OnUpdate();
		
		FlatEngine::Renderer::GetGbuffer()->End();
		
		FlatEngine::Renderer::GetSSAOBuffer()->BeginSSAOTexture(projection, FlatEngine::Renderer::GetGbuffer()->gPosition, FlatEngine::Renderer::GetGbuffer()->gNormal);
		FlatEngine::Draw::RenderQuad();
		FlatEngine::Renderer::GetSSAOBuffer()->EndSSAOTexture();

		// 3. blur SSAO texture to remove noise
		FlatEngine::Renderer::GetSSAOBuffer()->BeginSSAOBlurTexture();
		FlatEngine::Draw::RenderQuad();
		FlatEngine::Renderer::GetSSAOBuffer()->EndSSAOBlurTexture();

		
		FlatEngine::Lighting::UpdateLighting();

		FlatEngine::Renderer::BeginRendering();
		FlatEngine::Draw::RenderQuad();
		FlatEngine::Renderer::EndRendering();

		FlatEngine::UI::DrawEditorUI();

        //finish imgui render.
        FlatEngine::UI::RenderImgui();

		glfwSwapBuffers(FlatEngine::Window::GetOpenGLWindow());
	}
	FlatEngine::UI::EndImgui();
	FlatEngine::App::Close();
	glfwTerminate();
	return 0;
}
//TODO: Move to input class
void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_F12) == GLFW_PRESS)
		FlatEngine::Window::ExitWindow(window);
	static bool cursorLockKeyDown = false;
	if (FlatEngine::Input::GetKeyDown(FlatEngine::Key::Escape) && !cursorLockKeyDown) {
		cursorLockKeyDown = true;
		if(FlatEngine::Input::GetCursorState() == FlatEngine::Input::CURSOR_STATE_NORMAL)
			FlatEngine::Input::SetCursorState(FlatEngine::Input::CURSOR_STATE_DISABLED, window);
		else
			FlatEngine::Input::SetCursorState(FlatEngine::Input::CURSOR_STATE_NORMAL, window);
	}		
	if(FlatEngine::Input::GetKeyUp(FlatEngine::Key::Escape)){
		cursorLockKeyDown = false;	
	}

	if (FlatEngine::Input::GetKey(FlatEngine::Key::W))
		FlatEngine::Editor::GetEditorCamera()->ProcessKeyboard(FlatEngine::FORWARD, FlatEngine::Timestep::GetDeltaTime());
	if (FlatEngine::Input::GetKey(FlatEngine::Key::S))
		FlatEngine::Editor::GetEditorCamera()->ProcessKeyboard(FlatEngine::BACKWARD, FlatEngine::Timestep::GetDeltaTime());
	if (FlatEngine::Input::GetKey(FlatEngine::Key::A))
		FlatEngine::Editor::GetEditorCamera()->ProcessKeyboard(FlatEngine::LEFT, FlatEngine::Timestep::GetDeltaTime());
	if (FlatEngine::Input::GetKey(FlatEngine::Key::D))
		FlatEngine::Editor::GetEditorCamera()->ProcessKeyboard(FlatEngine::RIGHT, FlatEngine::Timestep::GetDeltaTime());
}
//TODO: Move to input class
// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	FlatEngine::Editor::GetEditorCamera()->ProcessMouseMovement(xoffset, yoffset);
}
//TODO: Move to input
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	FlatEngine::Editor::GetEditorCamera()->ProcessMouseScroll(static_cast<float>(yoffset));
}
