#pragma once

#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H

#include "Animator.h"
#include "AnimatedMesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AnimatedModel
{
private:
    Assimp::Importer importer;
    Skeleton skeleton;
    bool isAnim = false;
    std::vector<AnimatedMesh> *meshes;
    CustomJoint rootJoint;
    int joinCount;
    Animator* animator;
    Animation* animation;
    float* deltaTime;

    std::vector<Animation> animations;
    //ASSIMP
    const aiScene* scene;
    std::vector<aiNode*> ai_nodes;
    std::vector<aiNodeAnim*> ai_nodes_anim;
    std::vector<Bone> bones;
    glm::mat4 globalInverseTransform;
    Skeleton skeletonImported;
    std::string directory;
    bool existTangent, existNormal;
    std::vector<Texture> textures_loaded;

    //GEOMETRY & MATERIAL
    void LoadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    AnimatedMesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName);
    void recursiveNodeProcess(aiNode* node);
    //ANIMATION
    void AnimNodeProcess();
    Bone* FindBone(std::string name);
    aiNode* FindAiNode(std::string name);
    aiNodeAnim* FindAiNodeAnim(std::string name);
    int FindBoneIDByName(std::string name);
    void CreateBoneStructure();
    void AddAnimation(Animation& in_anim);
    Animation* FindAnimation(std::string anim_to_find);
    void PlayAnimation(Animation& anim, bool loop = false, bool reset_to_start = false);
    void StopAnimating();
public:
    MaterialHandle matHandle;

    AnimatedModel();
    AnimatedModel(std::string path);
    AnimatedModel(std::vector<AnimatedMesh>* meshes, CustomJoint root, int jointCount);
    void Update();
    void DoAnimation(Animation* anim);
    glm::mat4* GetJoinTransfoms();
    void AddJointToArray(CustomJoint headJoint, glm::mat4* matrices);
    inline CustomJoint GetRootJoin() { return rootJoint; }
    void Render(std::vector<Light*> lights);
    void UpdateSkeleton();
    void SetDeltaTime(float* delta);
    void AddAnimation(Animation* anim);
    void AddLights(std::vector<Light*> lights);
    void AddCamera(Camera* cam);
};

#endif // !ANIMATEDMODEL_H

