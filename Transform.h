#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "glm/glm.hpp"
#include <vector>
//#include <stack>

class Transform
{
public:
    glm::vec3 postion, rotation, scale;
    glm::mat4 model;
    glm::mat4 finalModelMatrix;
    Transform* parent;
    std::vector<Transform*> childs;
    //std::stack<glm::mat4> modelHierarchy;
    unsigned int num_childs = 0;

    Transform();
    void AttachTo(Transform* parentTransform);
};

#endif
