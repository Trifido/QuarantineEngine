#include "Animation.h"

Animation::Animation(std::string in_name, glm::vec2 times, int in_priority)
{
    name = in_name;
    start_time = times.x;
    end_time = times.y;
    priority = in_priority;
}

Animation::Animation(float time, std::list<KeyFrame> frames)
{
    this->animationTime = time;
    this->keyFrames = frames;
}
