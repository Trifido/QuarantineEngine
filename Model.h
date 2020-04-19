#pragma once

#ifndef MODEL_H
#define MODEL_H

#include "HeadersRequeriments.h"
#include "Mesh.h"
#include "MaterialHandle.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model
{
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;
    bool existTangent = false;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName);
public:
    MaterialHandle matHandle;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection; 

    bool isSelectableModel = false;
    bool isSelectedModel = false;

    Model();
    Model(char* path);
    Model(float rawData[], unsigned int numVertex);
    Model(float rawData[], unsigned int numVertex, std::vector<Texture> textImages);
    void Draw(bool isOutline=false);

    void Rotation(float radians, glm::vec3 axis);
    void TranslationTo(glm::vec3 position);
    void ScaleTo(glm::vec3 scale);
    void ResetModel();

    void DeleteGPUInfo();
    void SetShininess(float shini);
    void AddTextures(std::vector<Texture> texts);

    void isSelectable(bool selectable);
    void isSelected(bool select);
    float checkClickMouse(glm::vec3 origin, glm::vec3 mousePosition);

    void AddMaterial(Material* mat);
    void AddMaterial(Material* mat, std::vector<Texture> textures);
    void AddShader(Shader* shader);
    void AddLight(std::vector<Light*> lights);
    void AddCamera(Camera* cam);
};

#endif
