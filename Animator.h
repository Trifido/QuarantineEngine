#pragma once

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Animation.h"
#include "Joint.h"

class Animator
{
private:
    //AnimatedModel entity;
    Animation currentAnimation;
    float animationTime = 0.f;
    float* deltaTime;

    void IncreaseAnimationTime();
    std::map<std::string, glm::mat4> CalculateCurrentAnimationPose();
    void ApplyPoseToJoints(std::map<std::string, glm::mat4> pose, CustomJoint joint, glm::mat4 parentTransform);
    KeyFrame* GetPreviusNextFrames();
    float CalculateProgression(KeyFrame previus, KeyFrame next);
    std::map<std::string, glm::mat4> InterpolatePoses(KeyFrame previus, KeyFrame next, float time);
public:
    Animator() {}
    //Animator(AnimatedModel model);
    void DoAnimation(Animation anim);
    void Update(CustomJoint root);
    inline void SetDeltaTime(float* deltaTime) { this->deltaTime = deltaTime; }
};

#endif // !ANIMATOR_H

