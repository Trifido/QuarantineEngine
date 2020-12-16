#pragma once

#ifndef ANIMATED_MESH_H
#define ANIMATED_MESH_H

#include "Skeleton.h"
#include "MaterialHandle.h"

class Skeleton;

struct VertexAnimated
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangents;
    glm::vec3 Bitangents;
    glm::vec4 Weight = glm::vec4(0.0f);
    glm::uvec4 Id = glm::uvec4(0);
};

class AnimatedMesh
{
private:
public:
    Material* material;
    Skeleton sceneLoaderSkeleton;
    std::vector<VertexAnimated> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> stripTrianglesIndices;
    unsigned int VAO, VBO, EBO;
    bool isTriangleIndexSystem = true;

    AnimatedMesh(std::vector<VertexAnimated> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, MaterialHandle& matHandle);
    void setupMesh();
    void Draw();
    void SetRenderMode();
    void ChangeIndexSystem(bool isCommonIndex);
};

#endif // !ANIMATED_MESH_H
