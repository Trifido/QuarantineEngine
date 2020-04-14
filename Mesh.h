#pragma once
#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "Shader.h"
#include "includes/stb_image.h"
#include <string>
#include <vector>

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
    std::vector<Texture> textures;
    //Material info
    float shininess;
    /*  Functions  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures);
    void Draw(Shader shader);
    void DrawRaw(Shader shader);
    void DelteGPUInfo();
    void inline SetShininess(float shini) { shininess = shini; }

    void DrawOutline(Shader shIn, Shader shOutline);

    //RayCast
    bool RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayVector, unsigned int idTri, float& outIntersectionPoint, glm::mat4 model);
   
private:
    /*  Render data  */
    unsigned int VAO, VBO, EBO;
    /*  Functions    */
    void setupMesh();
};


#endif
