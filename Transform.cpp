#include "Transform.h"

#include "glm/gtc/matrix_transform.hpp"

CustomTransform::CustomTransform()
{
    parent = nullptr;
    finalModelMatrix = model = glm::mat4(1.0f);
    modelPos = modelRot = modelRotY = modelRotZ = modelScal = glm::mat4(1.0f);

    rawposition = position = glm::vec3(0.0);
    rawscale = scale = glm::vec3(1.0);
    rawrotation = rotation = glm::vec3(0.0);
}

void CustomTransform::AttachTo(CustomTransform* parentTransform)
{
    parent = parentTransform;
    parentTransform->childs.push_back(this);
    parentTransform->num_childs = childs.size();
}

void CustomTransform::CheckChanges()
{
    bool isChanged = false;

    if (this->position != rawposition)
    {
        this->position = rawposition;
        modelPos = glm::translate(glm::mat4(1.0), this->position);

        rawposition = this->position;
        isChanged = true;
    }
    if (this->rotation != rawrotation)
    {
        if (this->rotation.x != rawrotation.x)
        {
            modelRot = glm::rotate(glm::mat4(1.0), glm::radians(rawrotation.x), glm::vec3(1.0, 0.0, 0.0));
        }
        if (this->rotation.y != rawrotation.y)
        {
            modelRotY = glm::rotate(glm::mat4(1.0), glm::radians(rawrotation.y), glm::vec3(0.0, 1.0, 0.0));
        }
        if (this->rotation.z != rawrotation.z)
        {
            modelRotZ = glm::rotate(glm::mat4(1.0), glm::radians(rawrotation.z), glm::vec3(0.0, 0.0, 1.0));
        }

        modelRot *= modelRotY * modelRotZ;
        this->rotation = rawrotation;
        rawrotation = this->rotation;
        isChanged = true;
    }
    if (this->scale != rawscale)
    {
        this->scale = rawscale;
        modelScal = glm::scale(glm::mat4(1.0), this->scale);
        rawscale = this->scale;
        isChanged = true;
    }

    if (isChanged)
    {
        model = modelPos * modelRot * modelScal;
    }
}

void CustomTransform::AssignPhysicMatrix(float* data, glm::vec3 scaleFactor)
{
    glm::mat4 mScalingMatrix = glm::mat4(scaleFactor.x, 0, 0, 0,
        0, scaleFactor.y, 0, 0,
        0, 0, scaleFactor.z, 0,
        0, 0, 0, 1);

    glm::mat4 physicModel = glm::mat4(data[0], data[1], data[2], data[3],
                                      data[4], data[5], data[6], data[7],
                                      data[8], data[9], data[10], data[11],
                                      data[12], data[13], data[14], data[15]);

    finalModelMatrix = physicModel * mScalingMatrix;
}
