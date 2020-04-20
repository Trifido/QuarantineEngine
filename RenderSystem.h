#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "FBOSystem.h"
#include "UBOSystem.h"
#include "RenderPlane.h"
#include "KeyInput.h"
#include "SkyBox.h"

#include <GLFW/glfw3.h>

class RenderSystem
{
private:
    Skybox* skybox;
    std::vector<Model*> solidModels;
    std::vector<Model*> outLineModels;
    std::vector<Model*> transparentModels;
    KeyInput inputSystem;
    GLFWwindow* window;
    ImVec4* clear_color;
    FBOSystem* fboSystem;
    UBOSystem* uboSytem;
    RenderPlane renderFinalPass;
    int width, height;
    int lastWidth, lastHeight;

    void UpdateFBO();
    void Clean();
public:
    float deltaTime;	// Time between current frame and last frame
    float lastFrame;    // Time of last frame

    std::vector<Light*> lights;
    std::vector<Camera*> cameras;
    std::vector<Model*> models;
    
    RenderSystem()
    {
        deltaTime = 0;
        lastFrame = 0.0f;
    }

    RenderSystem(GLFWwindow* window, ImVec4* clearColor);

    void ComputeDeltaTime();

    static void GetWindowSize(GLFWwindow* window, int* width, int* height)
    {
        glfwGetWindowSize(window, width, height);
    }

    void AddGlfWindow(GLFWwindow* window);
    void AddLight(Light* lights);
    void AddCamera(Camera* cam);
    void AddModel(Model* model3D);

    void StartRender();
    void RenderSkyBox();
    void RenderSolidModels();
    void RenderOutLineModels();
    void RenderTransparentModels();
    void PreRender();
    void SetAmbientReflectiveMaterials();
};


