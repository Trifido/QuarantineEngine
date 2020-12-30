#pragma once

#ifndef RENDERVOLUME_H
#define RENDERVOLUME_H

#include "Transform.h"
#include "Mesh.h"
#include "IntersectionED.h"
#include "HeadersRequeriments.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

# define M_PI           3.14159265358979323846

enum RenderVolumeType
{
    BOX_VOL ,
    SPHERE_VOL,
    MESH_VOL
};

class RenderVolume
{
public:
    const aiScene* scene;
    std::vector<aiNode*> ai_nodes;
    RenderVolumeType type;
    CustomTransform* transform;
    Mesh meshCollider;
    MaterialHandle matHandle;
    float radius;
    glm::vec3 aabb_min, aabb_max;
    Camera* cam;
    std::vector<ProceduralTexture*> procedural_textures;
    ProceduralTexture* perlin;
    ProceduralTexture* worley;

    RenderVolume();
    RenderVolume(RenderVolumeType typeVolume, NoiseType type_procedural_texture = NoiseType::ALL);
    ~RenderVolume(){}
    void DrawRenderVolume();
    void AddCamera(Camera* cam);
private:
    void LoadMesh(std::string path);
    void recursiveNodeProcess(aiNode* node);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void SetHierarchy();
};


#endif
