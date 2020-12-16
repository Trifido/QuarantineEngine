#pragma once
#ifndef SKELETON_H
#define SKELETON_H

#include "Bone.h"
#include <vector>
#include <glm/glm.hpp>

class Skeleton
{
public:
    std::vector<Bone> bones;
    glm::mat4 globalInverseTransform;
    std::vector<glm::mat4> boneMats;

    float time;

    float start_time;
    float end_time;

    Animation* active_animation;
    Animation* idle_animation;

    bool anim_play;
    bool anim_loop;

    float* deltaTime;

    Skeleton();
    Skeleton(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform);
    void Init(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform);
    Bone* FindBone(std::string name);
    void UpdateBoneMatsVector();
    void Update();
    void PlayAnimation(Animation& anim, bool loop, bool reset_to_start);
    void StopAnimating();
    void SetIdleAnimation(Animation* in_anim);
    void SetDeltaTime(float* delta) { deltaTime = delta; }
};

#endif
