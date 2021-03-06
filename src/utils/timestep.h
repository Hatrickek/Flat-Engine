#pragma once
#include <GLFW/glfw3.h>
#include "core/core.h"
namespace FlatEngine{

    class Timestep{
    public:
    	static float GetDeltaTime() { return deltaTime;}
    	static void UpdateTime() {
		    float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
        }
    private:
        inline static float deltaTime = 0.0f;
		inline static float lastFrame = 0.0f;
    };
}
