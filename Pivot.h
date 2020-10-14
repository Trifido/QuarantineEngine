#pragma once

#ifndef PIVOT_H
#define PIVOT_H

#include "Model.h"

class Pivot
{
private:
    Model* pivotModel;
    Shader* pivotShader;

    float lastX, lastY;
public:
    bool isFirstDragX = true;
    bool isFirstDragY = true;
    bool activeDragX = false;
    bool activeDragY = false;
    bool activeDragZ = false;

    bool isRendered = false;
    Pivot();
    Model* GetModel() { return pivotModel; }
    void AttachModel(Model* model);
    void DrawPivot();
    bool CheckXAxis(glm::vec2 clickPosition, UIRay* ray);
    bool CheckYAxis(glm::vec2 clickPosition, UIRay* ray);
    bool CheckZAxis(glm::vec2 clickPosition, UIRay* ray);
    bool CheckCollision(glm::vec2 clickPosition, UIRay* ray);
};

#endif
