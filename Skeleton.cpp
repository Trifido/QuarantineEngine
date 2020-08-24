#include "Skeleton.h"

Skeleton::Skeleton(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
{
    Init(in_bones, in_globalInverseTransform);
}

void Skeleton::Init(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
{
    bones = in_bones;
    globalInverseTransform = in_globalInverseTransform;

    for (int i = 0; i < bones.size(); i++)
        bones.at(i).parent_skeleton = this;
}

Bone* Skeleton::FindBone(std::string name)
{
    for (int i = 0; i < bones.size(); i++)
    {
        if (bones.at(i).name == name)
            return &bones.at(i);
    }
    return nullptr;
}

void Skeleton::UpdateBoneMatsVector()
{
    if (bones.size() == 0)
        return;

    boneMats.clear();

    for (int i = 0; i < 100; i++)
    {
        if (i > bones.size() - 1)
            boneMats.push_back(glm::mat4(1.0));
        else
        {
            glm::mat4 concatenated_transformation = (bones.at(i).GetParentTransforms() * AiToGLMMat4(bones.at(i).node->mTransformation));
            boneMats.push_back(globalInverseTransform * concatenated_transformation * bones.at(i).offset_matrix);
        }
    }
}

void Skeleton::Update()
{
    UpdateBoneMatsVector();
}
