#pragma once

#ifndef COLLIDER_H
#define COLLIDER_H

#include "Transform.h"
#include "Mesh.h"
#include "HeadersRequeriments.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

enum ColliderType
{
    BOX,
    SPHERE,
    MESH
};

class Collider
{
public:
    const aiScene* scene;
    std::vector<aiNode*> ai_nodes;
    ColliderType type;
    Transform *transform;
    Mesh meshCollider;
    MaterialHandle matHandle;


    Collider();
    Collider(ColliderType typeCollider);
    void DrawCollider();

private:
    void LoadMesh(std::string path);
    void recursiveNodeProcess(aiNode* node);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif
