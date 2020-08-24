#include "Bone.h"
#include <vector>

Bone::Bone(unsigned int in_id, std::string in_name, aiMatrix4x4 in_o_mat)
{
    id = in_id;
    name = in_name;
    offset_matrix = AiToGLMMat4(in_o_mat);

    parent_bone = nullptr;
    node = nullptr;
}
//This constructor is similar to the last one in the sense that it is
//virtually identical and whose existence seems almost pointless. Almost.
Bone::Bone(unsigned int in_id, std::string in_name, glm::mat4 in_o_mat)
{
    id = in_id;
    name = in_name;
    offset_matrix = in_o_mat;

    parent_bone = nullptr;
    node = nullptr;
}
//Finally! An actual function!
//This function simply concatenates the parents' transforms.
//This is vital for calculating this bone's final transformation.
glm::mat4 Bone::GetParentTransforms()
{
    Bone* b = parent_bone;    //In order to recursively concatenate the transforms,
                       //we first start with this bone's parent.
    std::vector<glm::mat4> mats;    //Where we'll store the concatenated transforms.

    while (b != nullptr)    //As long as 'b' has a parent (see the end of the loop
    {                      //to avoid confusion).
        glm::mat4 tmp_mat = AiToGLMMat4(b->node->mTransformation); //This bone's transformation.
        mats.push_back(tmp_mat);

        b = b->parent_bone;    //We set b to its own parent so the loop can continue.
    }

    glm::mat4 concatenated_transforms;
    //IMPORTANT!!!! This next loop must be done in reverse, 
    //as multiplication with matrices is not commutative.
    for (int i = mats.size() - 1; i >= 0; i--)
        concatenated_transforms *= mats.at(i);

    return concatenated_transforms;    //Finally, we return the concatenated transforms.
}
