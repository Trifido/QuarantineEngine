#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "glm/glm.hpp"
#include <vector>
//#include <stack>

class Transform
{
public:
    glm::vec3 position, rotation, scale;
    glm::vec3 rawposition, rawrotation, rawscale;
    glm::mat4 model, modelPos, modelRot, modelRotY, modelRotZ, modelScal;
    glm::mat4 finalModelMatrix;
    Transform* parent;
    std::vector<Transform*> childs;
    //std::stack<glm::mat4> modelHierarchy;
    unsigned int num_childs = 0;

    Transform();
    void AttachTo(Transform* parentTransform);
    float* RawPosition() { return &rawposition[0]; }
    float* RawRotation() { return &rawrotation[0]; }
    float* RawScale() { return &rawscale[0]; }

    void CheckChanges();

};

#endif
