#pragma once

#ifndef PIVOT_H
#define PIVOT_H

#include "Model.h"

class Pivot
{
private:
    Model* pivotModel;
    Shader* pivotShader;
    bool firstMouse = false;
    float lastX, lastY;
public:
    bool isRendered = false;
    Pivot();
    Model* GetModel() { return pivotModel; }
    void AttachModel(Model* model);
    void DrawPivot();
    void CheckXAxis(glm::vec2 clickPosition, UIRay* ray);
    void CheckYAxis(glm::vec2 clickPosition);
};

#endif
