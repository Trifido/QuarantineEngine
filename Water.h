#pragma once
#ifndef WATER_H
#define WATER_H

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Water
{
private:
    const aiScene* scene;
    std::vector<aiNode*> ai_nodes;
    std::vector<Mesh> meshes;
    std::vector<Texture> textImages;
    std::string directory;
    bool existTangent = false;
    bool existNormal = false;
    Texture *distortionUV;
    Texture *normalUV;
    float moveFactor = 0.0f;
    int tilingValue = 6;
    float waveSpeed = 0.03f;
    glm::vec3 waterColor = glm::vec3(0.0f, 0.3f, 0.5f);
    float colorFactor = 0.2f;
    float factor_refractive = 1.0f;
    float *deltaTime;
    void AddPlane(float rawData[], unsigned int numVertex);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void recursiveNodeProcess(aiNode* node);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName);
public:
    Shader* waterShader;
    Transform *transform;

    Water();
    Water(std::string path);
    void Render(std::vector<Light*> lights, unsigned int reflectTexture, unsigned int refractTexture);
    void loadModel(std::string path);
    void AddCamera(Camera* mainCamera);
    void AddLights(std::vector<Light*> lights);
    void SetModelHierarchy();
    void ActivateIrradianceMap(unsigned int idTexIrradiance, unsigned int idTexPrefilter, unsigned int idTexBrdf);
    void ActivateShadowMap(unsigned int idTexShadow, unsigned int idLight, TypeLight type);
    int* GetTiling() { return &tilingValue; }
    float* GetWaveSpeed() { return &waveSpeed; }
    float* GetWaterColor() { return &waterColor[0]; }
    float* GetWaterColorFactor() { return &colorFactor; }
    float* GetWaterRefractiveFactor() { return &factor_refractive; }
    void SetDelta(float* delta) { deltaTime = delta; }
    void DeleteGPUInfo();
};

#endif
