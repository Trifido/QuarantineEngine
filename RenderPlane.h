#pragma once

#ifndef RENDERPLANE_H
#define RENDERPLANE_H

#include "Shader.h"
#include "FBOSystem.h"

class RenderPlane
{
private:
    float gammaValue;
    float exposureValue;
    float bloomValue;
    bool pingpongIdPass;
    unsigned int quadVAO, quadVBO;
    Shader* screenRenderShader;
    Shader* shaderBlur;
    Shader* shaderBloomFinal;
    FBOSystem* fboSystem;
public:
    RenderPlane();
    void SetVAORenderPlane();
    void FinalRenderPass();
    void FinalRenderBloom();
    void RenderBlur();
    void inline SetGamma(float value) { gammaValue = value; };
    void inline SetExposure(float value) { exposureValue = value; };
    void inline SetFBOSystem(FBOSystem* fboSy) { fboSystem = fboSy; }
};

#endif
