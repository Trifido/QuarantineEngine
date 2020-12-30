#pragma once
#ifndef GODRAY
#define GODRAY

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class GodRay
{
private:
    const aiScene* scene;
    std::vector<aiNode*> ai_nodes;
    std::vector<Mesh> meshes;
    std::vector<Texture> textImages;
    std::string directory;
    bool existTangent = false;
    bool existNormal = false;
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void recursiveNodeProcess(aiNode* node);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName);
    void DeleteGPUInfo();
public:
    bool isActive = true;
    CustomTransform* transform;
    Shader* godRayShader;
    GodRay();
    GodRay(std::string path);
    void SetModelHierarchy();
    void Render();
    void ScaleTo(glm::vec3 scale);
};

#endif // !GODRAY

