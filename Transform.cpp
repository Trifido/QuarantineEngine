#include "Transform.h"

Transform::Transform()
{
    parent = nullptr;
    finalModelMatrix = model = glm::mat4(1.0f);
}

void Transform::AttachTo(Transform* parentTransform)
{
    parent = parentTransform;
    parentTransform->childs.push_back(this);
    parentTransform->num_childs = childs.size();
}
