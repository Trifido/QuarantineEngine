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

class Mesh {
public:
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //Material info
    Material *material;
    /*  Functions  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, MaterialHandle &matHandle);
    Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, MaterialHandle& matHandle);
    void Draw(bool isOutline=false, bool isActive=false);
    void DrawRaw(Shader shader);
    void DelteGPUInfo();
    void inline SetShininess(float shini) { material->shininess = shini; }

    void DrawOutline(Shader shIn, Shader shOutline);

    //RayCast
    bool RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayVector, unsigned int idTri, float& outIntersectionPoint, glm::mat4 model);

    //void AddMaterial(Material* mat) { material = mat; }
   
private:
    /*  Render data  */
    unsigned int VAO, VBO, EBO;
    /*  Functions    */
    void setupMesh();
};


#endif
