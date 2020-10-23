#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Camera.h"
#include "Light.h"
#include "BoundingLight.h"
#include "Model.h"
#include "Pivot.h"
#include "FBOSystem.h"
#include "UBOSystem.h"
#include "SSBOSystem.h"
#include "RenderPlane.h"
#include "KeyInput.h"
#include "SkyBox.h"
#include "GUISystem.h"

#include <GLFW/glfw3.h>

class RenderSystem
{
private:
    GUISystem* guiSystem;
    RenderType renderMode = RenderType::DEFERRED_RENDER;
    //RenderType renderMode = RenderType::FORWARD_QUALITY_RENDER;
    Skybox* skybox, *precookSkybox;
    std::vector<Model*> solidModels;
    std::vector<Model*> fpsModels;
    std::vector<Model*> outLineModels;
    std::vector<Model*> transparentModels;
    std::vector<Model*> internalModels;
    std::vector<BoundingLight*> boundingModels;
    std::vector<Light*> shadowCastDirLights;
    std::vector<Light*> shadowCastOmniLights;
    std::vector<Light*> shadowCastSpotLights;
    std::vector<Light*> shadowCastGeneralLights;
    KeyInput inputSystem;
    Pivot* pivot;
    GLFWwindow* window;
    ImVec4* clear_color;
    FBOSystem* fboSystem;
    UBOSystem* uboSytem;
    SSBOSystem* ssboSystem;
    RenderPlane renderPass;
    int width, height;
    int lastWidth, lastHeight;
    const unsigned int LIMIT_DIR_CAST_SHADOW = 8;
    const unsigned int LIMIT_OMNI_CAST_SHADOW = 8;
    const unsigned int LIMIT_SPOT_CAST_SHADOW = 8;
    unsigned int num_dir_cast_shadow;
    unsigned int num_omni_cast_shadow;
    unsigned int num_spot_cast_shadow;
    float gamma;

    void UpdateFBO();
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
    void Clean();
    void AddLight(Light* lights);
    void AddLight(BoundingLight* lights);
    void AddCamera(Camera* cam);
    void AddModel(Model* model3D);
    void AddPreCookSkybox(Skybox* skyHDR);

    void SetRenderMode();
    void ForwardRender();
    void DefferedRender();
    void StartRender();
    void RenderSkyBox();
    void RenderFPSModels();
    void RenderDirectionalShadowMap();
    void RenderOmnidirectionalShadowMap();
    void RenderVolumeShadow();
    void RenderSolidModels();
    void RenderOutLineModels();
    void RenderTransparentModels();
    void RenderInternalModels();
    void RenderOcclusionLightScattering();
    void ProcessBoundingModels();
    void PreRender();
    void PreRenderHDRSkybox();
    void PrefilerPass();
    void BRDFPass();
    void SetAmbientReflectiveMaterials();

    //Output Scene
    void SaveScene();
};


