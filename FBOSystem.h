#pragma once
#ifndef FBOSYSTEM_H
#define FBOSYSTEM_H

#include "FBO.h"


class FBOSystem
{
private:
    FBO* omniFBO;
    FBO* dirFBO;
    FBO* colorFBO;
    FBO* mrtFBO;
    FBO* pingpongFBO;
    FBO* deferredFBO;
    FBO* lightVolumeFBO;
    FBO* ssaoFBO;
    FBO* skyboxFBO;
    FBO* prefilterFBO;
    FBO* volumeShadowFBO;
    FBO* lightScatteringFBO;
    FBO* depthMapFBO;
    FBO* waterFBO;
    int *width, *height;
public:
    FBOSystem(int *width, int *height);
    FBO* GetFBO(FBOType type);
    void AddFBO(FBO* fbo);
    void OmniShadowPass(unsigned int idPass);
    void DirShadowPass(unsigned int idPass);
    void PingPongPass(unsigned int idPass);
    void MRTPass(unsigned int idFBO = 0);
    void SSAOPass(unsigned int idFBO = 0);
    void FinalPass();
    void DeferredGeometryPass();
    void LightVolumePass();
    void MultisamplingPass();
    void SkyboxProcessPass();
    void VolumeShadowPass();
    void LightScatteringPass();
    void DepthMapPass();
    void WaterPass(unsigned int idFBO);
    unsigned int GetFinalRender();
    unsigned int GetMRTRender(int idTex = 0);
    unsigned int GetPingPongRender(int idTex = 0);
    unsigned int GetOmniRender(unsigned int id = 0);
    unsigned int GetDeferredRender(unsigned int id = 0);
    unsigned int GetLightVolumeRender(unsigned int id = 0);
    unsigned int GetLightScatteringRender(unsigned int id = 0);
    unsigned int GetSSAORender(unsigned int id = 0);
    unsigned int GetSkyboxRender(unsigned int id = 0);
    unsigned int GetPrefilterRender(unsigned int id = 0);
	unsigned int GetDirRender(unsigned int id = 0);
	unsigned int GetVolumeShadowRender(unsigned int id = 0);
	unsigned int GetDepthMapRender(unsigned int id = 0);
	unsigned int GetWaterRender(unsigned int id = 0);
    void ResizeFBOs();
    void BlitDepthBuffer(FBOType readBuffer, FBOType drawBuffer);
    void SetIrradianceMap();
    void UpdateMRT(unsigned int numSamples);
    void UpdateDirShadowFbo(unsigned int numRTargets);
    void UpdateOmniShadowFbo(unsigned int numRTargets);
};

#endif
