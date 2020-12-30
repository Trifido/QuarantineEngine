#include "Animator.h"
#include <math.h>

void Animator::IncreaseAnimationTime()
{
    animationTime += *deltaTime;
    if (animationTime > currentAnimation.GetAnimationTime()) {
        animationTime = fmodf(animationTime, currentAnimation.GetAnimationTime());
    }
}

std::map<std::string, glm::mat4> Animator::CalculateCurrentAnimationPose()
{
    KeyFrame* frames = GetPreviusNextFrames();
    float progression = CalculateProgression(frames[0], frames[1]);
    return InterpolatePoses(frames[0], frames[1], progression);
}

void Animator::ApplyPoseToJoints(std::map<std::string, glm::mat4> pose, CustomJoint joint, glm::mat4 parentTransform)
{
    glm::mat4 currentLocalTransform = pose.at(joint.name);
    glm::mat4 currentTransform = glm::matrixCompMult(parentTransform, currentLocalTransform);//Matrix4f.mul(parentTransform, currentLocalTransform, null);
    for (CustomJoint childJoint : joint.children) {
        ApplyPoseToJoints(pose, childJoint, currentTransform);
    }
    currentTransform = glm::matrixCompMult(currentTransform, joint.GetInverseTransform());//Matrix4f.mul(currentTransform, joint.getInverseBindTransform(), currentTransform);
    joint.SetJointTransform(currentTransform);
}

KeyFrame* Animator::GetPreviusNextFrames()
{
    std::list<KeyFrame> allFrames = currentAnimation.GetKeyFrames();
    KeyFrame previousFrame = *allFrames.begin();
    KeyFrame nextFrame = *allFrames.begin();
    std::list<KeyFrame>::iterator it = ++allFrames.begin();

    for (it; it != allFrames.end(); it++) {
        nextFrame = *it;
        if (nextFrame.GetTimeStamp() > animationTime) {
            break;
        }
        previousFrame = *it;
    }

    return new KeyFrame[2]{ previousFrame, nextFrame };
}

float Animator::CalculateProgression(KeyFrame previus, KeyFrame next)
{
    float totalTime = next.GetTimeStamp() - previus.GetTimeStamp();
    float currentTime = animationTime - previus.GetTimeStamp();
    return currentTime / totalTime;
}

std::map<std::string, glm::mat4> Animator::InterpolatePoses(KeyFrame previus, KeyFrame next, float time)
{
    std::map<std::string, glm::mat4> currentPose;
    std::map<std::string, JoinTransform>::iterator itPrev = previus.GetPose().begin();
    std::map<std::string, JoinTransform>::iterator itNext = next.GetPose().begin();

    while(itPrev != previus.GetPose().end() && itNext != previus.GetPose().end())
    {
        std::string name = itPrev->first;
        JoinTransform currentTransform = JoinTransform::Interpolate(itPrev->second, itNext->second, time);
        currentPose.insert(std::pair<std::string, glm::mat4>(name, currentTransform.GetLocalTransform()));
        itPrev++;
        itNext++;
    }

    return currentPose;
}

//Animator::Animator(AnimatedModel model)
//{
//    entity = model;
//}

void Animator::DoAnimation(Animation anim)
{
    animationTime = 0.f;
    currentAnimation = anim;
}

void Animator::Update(CustomJoint root)
{
    IncreaseAnimationTime();
    std::map<std::string, glm::mat4> currentPose = CalculateCurrentAnimationPose();
    ApplyPoseToJoints(currentPose, root, glm::mat4());
}
