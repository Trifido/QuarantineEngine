#pragma once

#ifndef MODEL_H
#define MODEL_H

#include "HeadersRequeriments.h"
#include "Mesh.h"
#include "Transform.h"
#include "MaterialHandle.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

enum ModelType
{
    REGULAR_M,
    PROCEDURAL_M,
    INSTANCE_M
};

class Model
{
private:
    ModelType model_type;
    
    std::vector<Texture> textures_loaded;
    std::string directory;
    bool existTangent = false;
    bool existNormal = false;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName);

public:
    std::vector<Mesh> meshes;
    bool CAST_SHADOW = true;
    MaterialHandle matHandle;
    Transform* transform;
    glm::mat4* modelMatrices;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection; 

    bool isSelectableModel = false;
    bool isSelectedModel = false;
    //bool isProcedural = false;

    Model();
    Model(char* path); 
    Model(float rawData[], unsigned int numVertex, unsigned int offset = 8, ModelType model_type = ModelType::REGULAR_M, glm::vec2* instances = NULL);
    Model(float rawData[], unsigned int numVertex, std::vector<Texture> textImages);
    void Draw(bool isOutline=false);
	void DrawCastShadow(std::vector<Light*> lights, bool isOutline = false);
    void DrawShadow(glm::mat4 VPShadow);
    void DrawShadow(std::vector<glm::mat4>& shadowTransforms, glm::vec3& lightPos, float far_plane);
    //void Model::DrawCastShadow(Light* light, bool isOutline = false);

    void Rotation(float radians, glm::vec3 axis);
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
};

#endif
