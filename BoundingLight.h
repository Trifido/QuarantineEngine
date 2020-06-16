#pragma once

#ifndef BOUNDINGMODEL_H
#define BOUNDINGMODEL_H

#include "HeadersRequeriments.h"
#include "Mesh.h"
#include "MaterialHandle.h"
#include "Light.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class BoundingLight
{
private:
    
    std::string directory;
    bool existTangent = false;
    bool existNormal = false;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene); 

public:
    Mesh* mesh;
    Light* light;
    MaterialHandle matHandle;
    glm::mat4 model = glm::mat4(1.0f); 
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;  

    BoundingLight(); 
    void Draw(bool isOutline=false);  

    void Rotation(float radians, glm::vec3 axis);
    void TranslationTo(glm::vec3 position);
    void ScaleTo(glm::vec3 scale);
    void ResetModel();

    void DeleteGPUInfo();    

    void AddMaterial(Material* mat);
    void AddMaterial(Material* mat, std::vector<Texture> textures);
    void AddShader(Shader* shader); 
    void AddCamera(Camera* cam);
};

#endif
