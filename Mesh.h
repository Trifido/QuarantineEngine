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
};
struct TextureComponent {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureComponent> textures;
    /*  Functions  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureComponent> textures);
    void Draw(Shader shader);
private:
    /*  Render data  */
    unsigned int VAO, VBO, EBO;
    /*  Functions    */
    void setupMesh();
};


#endif
