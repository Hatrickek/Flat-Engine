#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "input.h"
#include "render/window.h"
#include "utils/timestep.h"
#include "utils/log.h"
namespace FlatEngine {
	enum CameraMovement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};
	enum class CameraMod {
		FREE,
		STATIC,
		FPS,
	};
	// Default camera values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 7.5f;
	const float SENSITIVITY = .1f;
	const float ZOOM = 45.0f;
	const float NEARCLIP = 0.1f;
	const float FARCLIP =  80.0f; 
	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera {
	public:
		// camera Attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		// camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		CameraMod cameraMod = CameraMod::FREE;

		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}

		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
			Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
			Position = glm::vec3(posX, posY, posZ);
			WorldUp = glm::vec3(upX, upY, upZ);
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}

		glm::mat4 GetProjectionMatrix() const {
			return glm::perspective(glm::radians(Zoom), (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT, NEARCLIP, FARCLIP);
		}
		void SetYaw(float value) {
			Yaw = value;
			updateCameraVectors();
		}
		void SetPitch(float value) {
			Pitch = value;
			updateCameraVectors();
		}
		// returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix() const {
			return glm::lookAt(Position, Position + Front, Up);
		}

		glm::vec3 SetPosition(glm::vec3 pos) {
			Position = pos;
			return Position;
		}
		void OnUpdate() {
			if(cameraMod == CameraMod::FREE) {
				static bool mouseLocked;
				if(Input::GetMouseButtonDown(Mouse::Button1)) {
					if(Input::GetKey(Key::W))
						ProcessKeyboard(FORWARD, Timestep::GetDeltaTime());
					if(Input::GetKey(Key::S))
						ProcessKeyboard(BACKWARD, Timestep::GetDeltaTime());
					if(Input::GetKey(Key::A))
						ProcessKeyboard(LEFT, Timestep::GetDeltaTime());
					if(Input::GetKey(Key::D))
						ProcessKeyboard(RIGHT, Timestep::GetDeltaTime());
					if(Input::GetCursorState() == Input::CursorState::NORMAL)
						mouseLocked = true;
					Input::SetCursorState(Input::CursorState::DISABLED, Window::GetOpenGLWindow());
				}
				else if(mouseLocked) {
					mouseLocked = false;
					Input::SetCursorState(Input::CursorState::NORMAL, Window::GetOpenGLWindow());
				}
				//TODO: Right now these are getting updated directly from glfw callbacks in Input class.
				// maybe update them here instead of there.
				// Ways to do it:
				// Dont call the functions below if the Input::GetMouseOffsetY() is not updated.
				//ProcessMouseScroll(Input::GetMouseScrollOffsetY());
				//ProcessMouseMovement(Input::GetMouseOffsetX(), Input::GetMouseOffsetY());
			}
		}
		// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(CameraMovement direction, float deltaTime) {
			//if(cameraState == CAMERASTATE_EDITOR && !Input::GetMouseButtonDown(Mouse::Button1)) return;
			float velocity = MovementSpeed * deltaTime;
			if(direction == FORWARD)
				Position += Front * velocity;
			if(direction == BACKWARD)
				Position -= Front * velocity;
			if(direction == LEFT)
				Position -= Right * velocity;
			if(direction == RIGHT)
				Position += Right * velocity;
		}

		// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
			if(Input::GetCursorState() == Input::CursorState::NORMAL) return;

			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if(constrainPitch) {
				if(Pitch > 89.0f)
					Pitch = 89.0f;
				if(Pitch < -89.0f)
					Pitch = -89.0f;
			}

			// update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset) {
			Zoom -= (float)yoffset;
			if(Zoom < 1.0f)
				Zoom = 1.0f;
			if(Zoom > 45.0f)
				Zoom = 45.0f;
		}

	private:
		// euler Angles
		float Yaw;
		float Pitch;
		// calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors() {
			// calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch) ) ;
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));
			// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
	};
}
