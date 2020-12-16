#pragma once
#ifndef MESH_H
#define MESH_H

#include "Material.h"
#include "MaterialHandle.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangents;
    glm::vec3 Bitangents;
    glm::vec4 Weight;
    glm::uvec4 Id;
};

struct ProceduralVertex
{
    glm::vec2 Position;
    glm::vec3 Color;
};

class Skeleton;

class Mesh {
public:
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<ProceduralVertex> proceduralVertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> stripTrianglesIndices;
    //Material info
    Material *material;
    /*  Functions  */
    Mesh() {};
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Shader* shader);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, MaterialHandle& matHandle);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, MaterialHandle &matHandle);
    Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, MaterialHandle& matHandle);
    Mesh(std::vector<ProceduralVertex> vertices, std::vector<unsigned int> indices, MaterialHandle& matHandle, glm::vec2* instances = NULL);

    void MeshCollider(std::vector<Vertex> vertices, std::vector<unsigned int> indices, MaterialHandle& matHandle);
    void ScaleMeshCollider(glm::vec3 scalevert);
    void Draw(bool isOutline=false, bool isActive=false);
    void DrawShadow();
    void DrawVolumeShadow();
    void DrawOcclusionScattering(Shader* sh);
    void DelteGPUInfo();
    void inline SetShininess(float shini) { material->shininess = shini; }

    void DrawOutline(Shader shIn, Shader shOutline);

    //RayCast
    bool RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayVector, unsigned int idTri, float& outIntersectionPoint, glm::mat4 model);

    void setUpInstanceMesh(unsigned int ID);
    void SetIntanceMesh();
    bool inline GetIsBoundingLight() { return isBoundingLight; }
    void SetBoundingLight(bool value);
    unsigned int GetVAO() { return VAO; }
    void ChangeIndexSystem(bool isCommonIndex);
private:
    bool isBoundingLight = false;
    bool isTriangleIndexSystem = true;
    /*  Render data  */
    unsigned int VAO, VBO, EBO;
    /*  Functions    */
    void setupMesh();
    void setupVolumetricMesh();
    void setupProceduralMesh(glm::vec2* instances = NULL);
    void SetRenderMode();
    void DetermineAdjacency();
};


#endif
