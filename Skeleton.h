#pragma once
#ifndef SKELETON_H
#define SKELETON_H

#include "Bone.h"
#include <vector>
#include <glm/glm.hpp>

class Bone;

class Skeleton
{
public:
    std::vector<Bone> bones;
    glm::mat4 globalInverseTransform;
    std::vector<glm::mat4> boneMats;

    Skeleton(){}
    Skeleton(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform);
    void Init(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform);
    Bone* FindBone(std::string name);
    void UpdateBoneMatsVector();
    void Update();
};

#endif
