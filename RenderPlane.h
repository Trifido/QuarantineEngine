#pragma once

#ifndef RENDERPLANE_H
#define RENDERPLANE_H

#include "Shader.h"

class RenderPlane
{
private:
    float gammaValue;
    float exposureValue;
public:
    unsigned int quadVAO, quadVBO; 
    Shader *screenRenderShader;
    RenderPlane();
    void SetVAORenderPlane();
    void DrawFrom(unsigned int& textureColorbuffer);
    void inline SetGamma(float value) { gammaValue = value; };
    void inline SetExposure(float value) { exposureValue = value; };
};

#endif
