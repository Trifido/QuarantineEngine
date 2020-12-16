#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include "KeyFrame.h"
#include <list>

class Animation
{
private:
    float animationTime;
    std::list<KeyFrame> keyFrames;



    static float TimeToFrame(float time);
    static glm::vec2 FramesToTime(glm::vec2 frames);
public:
    std::string name;
    float start_time;
    float end_time;
    int priority;

    Animation() { start_time = end_time = priority = 0; }
    Animation(std::string in_name, glm::vec2 times, int in_priority);
    Animation(float time, std::list<KeyFrame> frames);
    inline float GetAnimationTime() { return animationTime; }
    inline std::list<KeyFrame> GetKeyFrames() { return keyFrames; }
};

#endif // !ANIMATION_H
