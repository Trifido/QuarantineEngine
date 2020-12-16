#include "KeyFrame.h"

KeyFrame::KeyFrame(float timeStamp, std::map<std::string, JoinTransform> pose)
{
    this->timeStamp = timeStamp;
    this->pose = pose;
}
