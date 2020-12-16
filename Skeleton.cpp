#include "Skeleton.h"

Skeleton::Skeleton()
{
    time = start_time = end_time = 0;
    active_animation = nullptr;
    anim_loop = false;
}

Skeleton::Skeleton(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
{
    time = start_time = end_time = 0;
    active_animation = nullptr;
    anim_loop = false;

    Init(in_bones, in_globalInverseTransform);
}

void Skeleton::Init(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
{
    bones = in_bones;
    globalInverseTransform = in_globalInverseTransform;

    time = start_time = end_time = 0;
    active_animation = nullptr;
    idle_animation = nullptr;

    anim_loop = false;

    //for (int i = 0; i < bones.size(); i++)
    //    bones.at(i).parent_skeleton = this;
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
    if (!anim_play)
        //If we're not playing an animation, then just give up, do nothing.
        return;

    //Update the time variable by adding the delta time of the last frame
    //It's * 0.001f because the delta time is in milliseconds, and we 
    //need it in seconds.
    time += *deltaTime * 0.1f;

    //Make sure the time can't be less than our animation's start time.
    if (time < start_time)
        time = start_time;

    //Make sure the time can't be greater than our animation's end time.
    if (time > end_time)
    {
        if (anim_loop)
            //If looping is set, then loop!
            time = start_time;
        else
            //Else, give up.
            StopAnimating();
    }

    for (int i = 0; i < bones.size(); i++)
        bones.at(i).UpdateKeyframeTransform(time);
}

void Skeleton::PlayAnimation(Animation& anim, bool loop, bool reset_to_start)
{
    //If there's an animation currently playing
    if (active_animation != nullptr)
    {
        //And this animation is more important than the current one
        if (anim.priority < active_animation->priority)
            //Set the current animation to the one passed in.
            active_animation = &anim;
        else
            //Do nothing.
            return;
    }
    else
        //Set the current animation to the one passed in.
        active_animation = &anim;

    start_time = active_animation->start_time;
    end_time = active_animation->end_time;

    anim_play = true;
    anim_loop = loop;

    //The reset_to_start variable determines whether or not the animation
    //should restart upon playing.
    if (reset_to_start)
        time = active_animation->start_time;
}

void Skeleton::StopAnimating()
{
    time = end_time;
    active_animation = nullptr;
    anim_play = false;
}

void Skeleton::SetIdleAnimation(Animation* in_anim)
{
    idle_animation = in_anim;
}
