#pragma once

#ifndef JOINT_H
#define JOINT_H

#include <string>
#include <list>

#include "glm/glm.hpp"

class Joint
{
private:
    glm::mat4 animatedTransform;
    glm::mat4 localBindTransform;
    glm::mat4 inverseBindTransform;
public:
    unsigned int ID;
    std::string name;
    std::list<Joint> children;

    Joint();
    Joint(unsigned int ID, std::string name, glm::mat4 bindLocalTransform);
    inline void AddChild(Joint child) { children.push_back(child); }
    inline glm::mat4 GetJointTransform() { return animatedTransform; }
    inline void SetJointTransform(glm::mat4 transform) { animatedTransform = transform; }
    inline glm::mat4 GetInverseTransform() { return inverseBindTransform; }
    void CalculateInverseTransform(glm::mat4 transformParent);
};

#endif
