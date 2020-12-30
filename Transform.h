#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "glm/glm.hpp"
#include <vector>
//#include <stack>

class CustomTransform
{
public:
    glm::vec3 position, rotation, scale;
    glm::vec3 rawposition, rawrotation, rawscale;
    glm::mat4 model, modelPos, modelRot, modelRotY, modelRotZ, modelScal;
    glm::mat4 finalModelMatrix;
    CustomTransform* parent;
    std::vector<CustomTransform*> childs;
    //std::stack<glm::mat4> modelHierarchy;
    unsigned int num_childs = 0;

    CustomTransform();
    void AttachTo(CustomTransform* parentTransform);
    float* RawPosition() { return &rawposition[0]; }
    float* RawRotation() { return &rawrotation[0]; }
    float* RawScale() { return &rawscale[0]; }

    void CheckChanges();
    void AssignPhysicMatrix(float* data, glm::vec3 scaleFactor);
};

#endif
