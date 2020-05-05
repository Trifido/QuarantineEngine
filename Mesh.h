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
};

struct ProceduralVertex
{
    glm::vec2 Position;
    glm::vec3 Color;
};

class Mesh {
public:
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<ProceduralVertex> proceduralVertices;
    std::vector<unsigned int> indices;
    //Material info
    Material *material;
    /*  Functions  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, MaterialHandle &matHandle);
    Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, MaterialHandle& matHandle);
    Mesh(std::vector<ProceduralVertex> vertices, std::vector<unsigned int> indices, MaterialHandle& matHandle, glm::vec2* instances = NULL);
    void Draw(bool isOutline=false, bool isActive=false);
    void DrawShadow();
    void DelteGPUInfo();
    void inline SetShininess(float shini) { material->shininess = shini; }

    void DrawOutline(Shader shIn, Shader shOutline);

    //RayCast
    bool RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayVector, unsigned int idTri, float& outIntersectionPoint, glm::mat4 model);

    void setUpInstanceMesh(unsigned int ID);
    void SetIntanceMesh();
private:
    /*  Render data  */
    unsigned int VAO, VBO, EBO;
    /*  Functions    */
    void setupMesh(); 
    void setupProceduralMesh(glm::vec2* instances = NULL);
    void SetRenderMode();
};


#endif
