#include "Transform.h"

#include "glm/gtc/matrix_transform.hpp"

Transform::Transform()
{
    parent = nullptr;
    finalModelMatrix = model = glm::mat4(1.0f);
    modelPos = modelRot = modelRotY = modelRotZ = modelScal = glm::mat4(1.0f);

    rawposition = position = glm::vec3(0.0);
    rawscale = scale = glm::vec3(1.0);
    rawrotation = rotation = glm::vec3(0.0);
}

void Transform::AttachTo(Transform* parentTransform)
{
    parent = parentTransform;
    parentTransform->childs.push_back(this);
    parentTransform->num_childs = childs.size();
}

void Transform::CheckChanges()
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
