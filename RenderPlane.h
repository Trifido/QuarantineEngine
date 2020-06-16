#pragma once

#ifndef RENDERPLANE_H
#define RENDERPLANE_H

#include "Shader.h"
#include "FBOSystem.h"
#include "UBOSystem.h"
#include <random>

class RenderPlane
{
private:
    float gammaValue;
    float exposureValue;
    float bloomValue;
    bool pingpongIdPass;
    unsigned int amountBloom;
    unsigned int quadVAO, quadVBO;
    Shader* screenRenderShader;
    Shader* shaderBlur;
    Shader* shaderBloomFinal;
    Shader* deferredLighting;
    Shader* ssao;
    Shader* ssao_blur;
    FBOSystem* fboSystem;
    //SSAO
    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    unsigned int noiseTexture;
    Camera* cam;
    float lerp(float a, float b, float f);
public:
    RenderPlane();
    void SetVAORenderPlane();
    void FinalRenderPass();
    void FinalRenderBloom();
    void RenderBlur();
    void DefferedRender();
    void RenderSSAO();
    void RenderBlurSSAO();
    void inline SetGamma(float value) { gammaValue = value; };
    void inline SetExposure(float value) { exposureValue = value; };
    void inline SetFBOSystem(FBOSystem* fboSy) { fboSystem = fboSy; }
    void inline SetBloom(bool value) { bloomValue = value; }
    void inline SetAmountBloom(unsigned int value) { amountBloom = value; }
    void AddLights(std::vector<Light*> lights);
    void AddCamera(Camera* cam);
    void GenerateNoiseTexture();
    void AddUBOSystem(UBOSystem* ubo);
};

#endif
