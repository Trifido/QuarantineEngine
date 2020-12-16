#pragma once

#ifndef JOINTTRANSFORM_H
#define JOINTTRANSFORM_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

class JoinTransform
{
private:
    glm::vec3 position;
    glm::quat rotation;
public:
    JoinTransform() {}
    JoinTransform(glm::vec3 pos, glm::quat rot);
    glm::mat4 GetLocalTransform();
    static JoinTransform Interpolate(JoinTransform frameA, JoinTransform frameB, float time);
};

#endif // !JOINTTRANSFORM_H
