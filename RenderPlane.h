#pragma once

#ifndef RENDERPLANE_H
#define RENDERPLANE_H

#include "Shader.h"

class RenderPlane
{
public:
    unsigned int quadVAO, quadVBO; 
    Shader *screenRenderShader;
    RenderPlane();
    void SetVAORenderPlane();
    void DrawFrom(unsigned int& textureColorbuffer);
};

#endif
