#include "Joint.h"

CustomJoint::CustomJoint()
{
    animatedTransform = glm::mat4(1.0f);
    localBindTransform = glm::mat4(1.0f);
    inverseBindTransform = glm::mat4(1.0f);
}

CustomJoint::CustomJoint(unsigned int ID, std::string name, glm::mat4 bindLocalTransform)
{
    this->ID = ID;
    this->name = name;
    this->localBindTransform = bindLocalTransform;
}

void CustomJoint::CalculateInverseTransform(glm::mat4 transformParent)
{
    glm::mat4 bindTransform = transformParent * localBindTransform;
    inverseBindTransform = glm::inverse(bindTransform);

    for (std::list<CustomJoint>::iterator it = children.begin(); it != children.end(); it++)
    {
        it->CalculateInverseTransform(bindTransform);
    }
}
