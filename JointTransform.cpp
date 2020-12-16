#include "JointTransform.h"

JoinTransform::JoinTransform(glm::vec3 pos, glm::quat rot)
{
    this->position = pos;
    this->rotation = rot;
}

glm::mat4 JoinTransform::GetLocalTransform()
{
    glm::mat4 transform = glm::mat4();
    transform = glm::translate(transform, position);
    transform = transform * glm::toMat4(rotation);

    return transform;
}

JoinTransform JoinTransform::Interpolate(JoinTransform frameA, JoinTransform frameB, float time)
{
    glm::vec3 pos = frameA.position * (1.f - time) + frameB.position * time; //frameA.position + (frameB.position - frameA.position) * time
    glm::quat rot = glm::lerp(frameA.rotation, frameB.rotation, time);
    return JoinTransform(pos, rot);
}
