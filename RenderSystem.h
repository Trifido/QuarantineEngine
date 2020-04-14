#pragma once

#include <GLFW/glfw3.h>

class RenderSystem
{
public:
    static float deltaTime;	// Time between current frame and last frame
    static float lastFrame; // Time of last frame
     
    
    RenderSystem()
    {
        deltaTime = 0;
        lastFrame = 0.0f;  
    }

    static void ComputeDeltaTime()
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 
    }

    static void GetWindowSize(GLFWwindow* window, int* width, int* height)
    {
        glfwGetWindowSize(window, width, height);
    }
};


