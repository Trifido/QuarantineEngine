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
    std::vector<Light*> shadowCastDirLights;
    std::vector<Light*> shadowCastOmniLights;
    std::vector<Light*> shadowCastSpotLights;
    std::vector<Light*> shadowCastGeneralLights;
    KeyInput inputSystem;
    GLFWwindow* window;
    ImVec4* clear_color;
    FBOSystem* fboSystem;
    //FBOSystem* fboSystemShadowMap;
    UBOSystem* uboSytem;
    RenderPlane renderPass;
    int width, height;
    int lastWidth, lastHeight;
    const unsigned int LIMIT_DIR_CAST_SHADOW = 5;
    const unsigned int LIMIT_OMNI_CAST_SHADOW = 5;
    const unsigned int LIMIT_SPOT_CAST_SHADOW = 5;
    unsigned int num_dir_cast_shadow;
    unsigned int num_omni_cast_shadow;
    unsigned int num_spot_cast_shadow;
    float gamma;

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
    void RenderDirectionalShadowMap();
    void RenderOmnidirectionalShadowMap();
    void RenderSolidModels();
    void RenderOutLineModels();
    void RenderTransparentModels();
    void PreRender();
    void SetAmbientReflectiveMaterials();
};


