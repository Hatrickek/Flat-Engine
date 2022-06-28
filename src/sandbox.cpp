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

#include "render/renderer.h"
#include "ui/panels/viewportPanel.h"

//**TODO: Move to input handling 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);
//**

// camera
FlatEngine::Camera camera;
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
	FlatEngine::Shader shader_lighting_pass(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_lighting.fs").c_str());
	FlatEngine::Shader shader_ssao(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao.fs").c_str());
	FlatEngine::Shader shader_ssao_blur(FileSystem::getPath("resources/shaders/ssao.vs").c_str(), FileSystem::getPath("resources/shaders/ssao_blur.fs").c_str());

	FlatEngine::Renderer::GetSSAOBuffer()->SetupSSAOShader(&shader_ssao, &shader_ssao_blur);

	// lighting info
	lightPositions.emplace_back(glm::vec3(2.0, 1.0, -2.0));
	lightColors.emplace_back(glm::vec3(0.2, 0.2, 0.7));
	// shader configuration
	shader_lighting_pass.use();
	shader_lighting_pass.setInt("gPosition", 0);
	shader_lighting_pass.setInt("gNormal", 1);
	shader_lighting_pass.setInt("gAlbedo", 2);
	shader_lighting_pass.setInt("ssao", 3);
	
	// render loop
	// -----------
	camera = *FlatEngine::Editor::GetEditorCamera();

	camera.SetPosition(glm::vec3(1,1,3));

	while (!glfwWindowShouldClose(FlatEngine::Window::GetOpenGLWindow())) {
		FlatEngine::App::Update();

		// input
		// -----
		ProcessInput(FlatEngine::Window::GetOpenGLWindow());
		//patrick_model.UpdateAnimation(FlatEngine::Timestep::GetDeltaTime());

		FlatEngine::UI::UpdateImgui();
		// render
		// ------
		FlatEngine::Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		FlatEngine::Renderer::Clear();

		FlatEngine::Renderer::GetGbuffer()->Begin();

		glm::mat4 projection = camera.GetProjectionMatrix();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shader_ssao_geometry_pass.use();
		shader_ssao_geometry_pass.setMat4("projection", projection);
		shader_ssao_geometry_pass.setMat4("view", view);

		shader_ssao_geometry_pass.setInt("invertedNormals", 1);
		FlatEngine::Draw::DrawCube(shader_ssao_geometry_pass, WHITE,glm::vec3(0.0, 7.0f, 0.0f), glm::vec3(7.5f, 7.5f, 7.5f));
		shader_ssao_geometry_pass.setInt("invertedNormals", 0);

		FlatEngine::Editor::OnUpdate();
		
		//Draw the lights as cubes
		for (unsigned int i = 0; i < lightPositions.size(); i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(lightPositions[i]));
			model = glm::scale(model, glm::vec3(0.1f));
			shader_ssao_geometry_pass.setMat4("model", model);
			FlatEngine::Draw::DrawCube(shader_ssao_geometry_pass, glm::vec4(lightColors[i], 1), lightPositions[i], glm::vec3(.1,.1,.1));
		}
		FlatEngine::Renderer::GetGbuffer()->End();
		
		FlatEngine::Renderer::GetSSAOBuffer()->BeginSSAOTexture(projection, FlatEngine::Renderer::GetGbuffer()->gPosition, FlatEngine::Renderer::GetGbuffer()->gNormal);
		FlatEngine::Draw::RenderQuad();
		FlatEngine::Renderer::GetSSAOBuffer()->EndSSAOTexture();

		// 3. blur SSAO texture to remove noise
		FlatEngine::Renderer::GetSSAOBuffer()->BeginSSAOBlurTexture();
		FlatEngine::Draw::RenderQuad();
		FlatEngine::Renderer::GetSSAOBuffer()->EndSSAOBlurTexture();

		//TODO: Lighting class 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_lighting_pass.use();
		// send light relevant uniforms
		glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPositions[0], 1.0));
		shader_lighting_pass.setVec3("light.Position", lightPosView);
		shader_lighting_pass.setVec3("light.Color", lightColors[0]);
		// Update attenuation parameters
		const float linear = 0.09f;
		const float quadratic = 0.032f;
		shader_lighting_pass.setFloat("light.Linear", linear);
		shader_lighting_pass.setFloat("light.Quadratic", quadratic);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetGbuffer()->gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetGbuffer()->gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetGbuffer()->gAlbedo);
		glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
		glBindTexture(GL_TEXTURE_2D, FlatEngine::Renderer::GetSSAOBuffer()->ssaoColorBufferBlur);
		FlatEngine::Renderer::BeginRendering();
		FlatEngine::Draw::RenderQuad();
		FlatEngine::Renderer::EndRendering();

		FlatEngine::UI::DrawEditorUI();

        //finish imgui render.
        FlatEngine::UI::RenderImgui();

		glfwSwapBuffers(FlatEngine::Window::GetOpenGLWindow());
	}
	FlatEngine::UI::EndImgui();

	glfwTerminate();
	return 0;
}
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
		camera.ProcessKeyboard(FlatEngine::FORWARD, FlatEngine::Timestep::GetDeltaTime());
	if (FlatEngine::Input::GetKey(FlatEngine::Key::S))
		camera.ProcessKeyboard(FlatEngine::BACKWARD, FlatEngine::Timestep::GetDeltaTime());
	if (FlatEngine::Input::GetKey(FlatEngine::Key::A))
		camera.ProcessKeyboard(FlatEngine::LEFT, FlatEngine::Timestep::GetDeltaTime());
	if (FlatEngine::Input::GetKey(FlatEngine::Key::D))
		camera.ProcessKeyboard(FlatEngine::RIGHT, FlatEngine::Timestep::GetDeltaTime());
}

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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
