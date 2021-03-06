#pragma once

#ifndef MODEL_H
#define MODEL_H

#include "HeadersRequeriments.h"
#include "Mesh.h"
#include "Bone.h"
#include "Skeleton.h"
#include "Transform.h"
#include "MaterialHandle.h"
#include "Collider.h"
#include "PhysicSystem.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

enum ModelType
{
    REGULAR_M,
    PROCEDURAL_M,
    INSTANCE_M
};

enum DEMO
{
    IS_ARM,
    IS_NOTHING,
    IS_WEAPON,
    IS_APPEARED,
    IS_ARTIFACT,
    IS_STATUE
};

class Model
{
private:
    const aiScene* scene;
    ModelType model_type;
    std::vector<Texture> textures_loaded;
    std::string directory;
    bool existTangent = false;
    bool existNormal = false;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName);
    //Animation
    bool anim;
    std::vector<Bone> bones;
    std::vector<aiNode*> ai_nodes;
    std::vector<aiNodeAnim*> ai_nodes_anim;
    Skeleton skeletonImported;

    PhysicBodyType physicType = PhysicBodyType::COLLISION_BODY;
    float modelMatrix[16];
    glm::vec3 scaleFactor = glm::vec3(1.0f);
    ColliderShapeType colliderType = ColliderShapeType::NULL_SHAPE;

    std::vector<unsigned int> rawIndices;
    std::vector<reactphysics3d::Vector3> rawVertices;
    unsigned int numFaces;
    unsigned int numVertices;
public:
    DEMO demoAttribute = DEMO::IS_NOTHING;
    std::vector<Mesh> meshes;
    std::vector<CustomCollider*> colliders;
    bool CAST_SHADOW = true;
    MaterialHandle matHandle;
    CustomTransform* transform;
    reactphysics3d::Transform* physicTransform;
    glm::mat4* modelMatrices;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection; 

    bool isSelectableModel = false;
    bool isSelectedModel = false;
    bool isCollider = false;

    float radius;
    reactphysics3d::Vector3 boxSize;
    reactphysics3d::Vector2 capsuleSize;

    Model();
    Model(char* path); 
    Model(float rawData[], unsigned int numVertex, unsigned int offset = 8, ModelType model_type = ModelType::REGULAR_M, glm::vec2* instances = NULL);
    Model(float rawData[], unsigned int numVertex, std::vector<Texture> textImages);
    void Draw(bool isOutline=false);
	void DrawCastShadow(std::vector<Light*> lights, bool isOutline = false);
    void DrawShadow(glm::mat4 VPShadow);
    void DrawShadow(std::vector<glm::mat4>& shadowTransforms, glm::vec3& lightPos, float far_plane);
    void DrawVolumeShadow(glm::vec3& lightPos);
    void DrawDepthMap(float nearp, float farp);
    void DrawOcclusion(std::vector<Light*> lights);
    inline void SetModelType(ModelType type) { model_type = type; }
    inline void SetPhysicType(PhysicBodyType type) { physicType = type; }
    inline PhysicBodyType GetPhysicType() { return physicType; }
    //void Model::DrawCastShadow(Light* light, bool isOutline = false);

    ColliderShapeType inline GetColliderType() { return this->colliderType; }
    void SetCollider(ColliderShapeType type);
    void SetCollider(ColliderShapeType type, float radius);
    void SetCollider(ColliderShapeType type, glm::vec3 boxSize);
    void SetCollider(ColliderShapeType type, glm::vec2 capsuleSize);
    void SetPositionOrientation(reactphysics3d::Vector3 position = reactphysics3d::Vector3(0.0f, 0.0f, 0.0f),
        reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity());
    void Rotation(float radians, glm::vec3 axis);
    void RotationTo(float radians, glm::vec3 axis);
    void TranslationTo(glm::vec3 position);
    void ScaleTo(glm::vec3 scale);
    void AttachModel(Model* modelParent);
    void AttachCamera(Camera* cameraParent);
    void SetModelHierarchy();
    void ResetModel();

    void DeleteGPUInfo();
    void SetShininess(float shini);
    void AddTextures(std::vector<Texture> texts);

    void SetInstanceModel(unsigned int ID);
    void SetIntanceModelMatrix();

    void isSelectable(bool selectable);
    void isSelected(bool select);
    void SetCastShadow(bool value);
    float checkClickMouse(glm::vec3 origin, glm::vec3 mousePosition);

    void AddMaterial(Material* mat);
    void AddMaterial(Material* mat, std::vector<Texture> textures);
    void AddShader(Shader* shader);
    void AddLight(std::vector<Light*> lights);
    void AddCamera(Camera* cam);
    void ChangeIndexSystem(bool indexSystem);

    void SetDemoType(DEMO demo);
    //ANIMATION
    Bone* FindBone(std::string name);
    aiNode* FindAiNode(std::string name);
    aiNodeAnim* FindAiNodeAnim(std::string name);
    int FindBoneIDByName(std::string name);

    void recursiveNodeProcess(aiNode* node);
    void AnimNodeProcess();

    std::vector<rp3d::Vector3> inline GetRawVertices() { return rawVertices; }
    std::vector<unsigned int> inline GetRawIndices() { return rawIndices; }
    unsigned int inline GetNumFaces() { return numFaces; }
    unsigned int inline GetNumVertices() { return numVertices; }
};

#endif
