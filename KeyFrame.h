#pragma once

#ifndef KEYFRAME_H
#define KEYFRAME_H

#include "JointTransform.h"
#include <map>
#include <string>

class KeyFrame
{
private:
    float timeStamp;
    std::map<std::string, JoinTransform> pose;
public:
    KeyFrame() {}
    KeyFrame(float timeStamp, std::map<std::string, JoinTransform> pose);
    inline float GetTimeStamp() { return timeStamp; }
    inline std::map<std::string, JoinTransform> GetPose() { return pose; }
};

#endif // !KEYFRAME_H
