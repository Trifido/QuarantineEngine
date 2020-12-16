#include "Joint.h"

Joint::Joint()
{
    animatedTransform = glm::mat4(1.0f);
    localBindTransform = glm::mat4(1.0f);
    inverseBindTransform = glm::mat4(1.0f);
}

Joint::Joint(unsigned int ID, std::string name, glm::mat4 bindLocalTransform)
{
    this->ID = ID;
    this->name = name;
    this->localBindTransform = bindLocalTransform;
}

void Joint::CalculateInverseTransform(glm::mat4 transformParent)
{
    glm::mat4 bindTransform = transformParent * localBindTransform;
    inverseBindTransform = glm::inverse(bindTransform);

    for (std::list<Joint>::iterator it = children.begin(); it != children.end(); it++)
    {
        it->CalculateInverseTransform(bindTransform);
    }
}
