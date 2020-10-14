#pragma once

#ifndef COLLIDER_H
#define COLLIDER_H

#include "Transform.h"
#include "Mesh.h"
#include "IntersectionED.h"
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
    float radius;
    glm::vec3 aabb_min, aabb_max;

    Collider();
    Collider(ColliderType typeCollider);
    void DrawCollider();

    bool IsRayCollision(UIRay *ray);
    float CheckCollider(glm::vec3 origin, glm::vec3 dir);
private:
    void LoadMesh(std::string path);
    void recursiveNodeProcess(aiNode* node);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void SetHierarchy();
    bool RaySphereIntersection(glm::vec3 ray_orig, glm::vec3 ray_dir);
    float RayBoxOBBIntersection(glm::vec3 ray_orig, glm::vec3 ray_dir);
};

#endif
