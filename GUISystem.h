#pragma once

#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include "MaterialHandle.h"
#include "Light.h"
#include "Model.h"
#include "Water.h"
#include "ParticleSystem.h"

struct BloomGUI
{
    bool enable = false;
    int blurRange = 20;

    BloomGUI() {}
};
struct HdrGUI
{
    float gammaParameter = 2.2f;
    float exposureParameter = 0.5f;

    HdrGUI() {}
};
struct AtmScatGUI
{
    bool isEnable = false;
    float decay = 0.96815f;
    float density = 0.587f;
    float weight = 0.926f;

    AtmScatGUI() {}
};
struct MsaaGUI
{
    unsigned int samplesOffScreenParameter = 8;
    unsigned int samplesMSAAParameter = 8;
    bool enableMSAA = false;
    bool enableOffMSAA = true;
    bool isModifiedMSAA = false;
    bool isModifiedOffMSAA = false;

    MsaaGUI() {}
};

static class GUISystem
{
private:
    RenderType typeRenderSelected = RenderType::FORWARD_QUALITY_RENDER;
    //RenderType typeRenderSelected = RenderType::DEFERRED_RENDER;
    bool isForward = true;
    bool isPBRForward = true;
    bool isShutdown = false;
    bool isSaveScene = false;

    bool isOpenBloomWindow = false;
    bool isOpenHDRWindow = false;
    bool isOpenOffScreeMSAAWindow = false;
    bool isOpenMSAAWindow = false;
    bool isOpenModelPropertyWindow = false;
    bool isOpenMaterialPropertyWindow = false;
    bool isOpenLightPropertyWindow = false;
    bool isOpenCameraPropertyWindow = false;
    bool isOpenAtmosphericPropertyWindow = false;
    bool isOpenFPS = false;
    bool isChangeShadow = false;
    bool isShowShadowVolume = false;
    bool isWaterTool = false;
    bool isPsTool = false;
    DrawMode draw_mode = DrawMode::DTRIANGLES;
    ShadowType shadow_mode = ShadowType::SHADOW_CAS;
    ShadowType last_shadow_mode = ShadowType::SHADOW_CAS;
public:
    BloomGUI* bloomGui;
    HdrGUI* HdrGui;
    MsaaGUI* msaaGui;
    AtmScatGUI* atmGUI;

    std::vector<Light*>* lights;
    std::vector<Camera*>* cameras;
    std::vector<Model*>* models;
    std::vector<Water*>* waterModels;
    std::vector<ParticleSystem*>* psModels;
    float clipPlaneH = -0.316f;

    GUISystem();
    void DrawGUI();
    void DrawMainMenuBar();
    void DrawPostProcessWindow();
    void DrawPropertyWindow();
    void DrawAnalysisWindow();
    void DrawEditWindow();
    void DrawParticleSystemWindow();
    RenderType GetRenderModeSelected();
    bool isShutDown();
    bool isSave();
    DrawMode GetDrawMode() { return draw_mode; }
    ShadowType GetShadowMode() { return shadow_mode; }
    BloomGUI* GetBloomParameters() { return bloomGui; }
    void SetBloomParameters(bool enableParam, int rangeParam);
    void SetHDRParameters(float gammaParam, float exposureParam);
    void SetAtmosphericParameters(float densityParam, float decayParam, float weightParam, bool isEnableLightScattering);
    void SetMSAAParameters(unsigned int msaaSamples, unsigned int offSamples, bool enableMSAA, bool enableOffMSAA);
    void SetLightInfoGui(std::vector<Light*>* lights);
    void SetCameraInfoGui(std::vector<Camera*>* cameras);
    void SetModelInfoGui(std::vector<Model*>* models);
    void SetWaterModelInfoGui(std::vector<Water*>* models);
    void SetParticleSystemInfoGui(std::vector<ParticleSystem*>* psModels);
    bool isChangeShadowMode() { return isChangeShadow; }
    bool isShowShadowVolumeMode() { return isShowShadowVolume; }
    bool IsLightScattering() { return atmGUI->isEnable; }
};

#endif
