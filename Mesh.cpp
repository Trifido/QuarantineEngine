#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    if (indices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangents));
    // vertex bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangents));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader shader)
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    unsigned int emissiveNr = 0;
    
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        //glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)

        std::string name;

        switch (textures[i].type)
        {
            default:
            case TypeTexture::DIFFUSE:
                name = "material.diffuse[" + std::to_string(diffuseNr++) + "]";
                break;
            case TypeTexture::SPECULAR:
                name = "material.specular[" + std::to_string(specularNr++) + "]";
                break;
            case TypeTexture::NORMAL:
                name = "material.normal[" + std::to_string(normalNr++) + "]";
                break;
            case TypeTexture::HEIGHT:
                name = "material.height[" + std::to_string(heightNr++) + "]";
                break;
            case TypeTexture::EMISSIVE:
                name = "material.emissive[" + std::to_string(emissiveNr++) + "]";
                break;
        }

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);

        shader.use();
        shader.setInt((name).c_str(), i);
    }

    shader.setInt("material.num_diffuse", diffuseNr);
    shader.setInt("material.num_specular", specularNr);
    shader.setInt("material.num_normal", normalNr);
    shader.setInt("num_normal", normalNr);
    shader.setInt("material.num_height", heightNr);
    shader.setInt("material.num_emissive", emissiveNr);
    shader.setFloat("material.shininess", shininess);

    shader.use();
    // draw mesh
    glBindVertexArray(VAO);
    //glPolygonMode(GL_FRONT, GL_FILL);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::DelteGPUInfo()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    //this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::DrawRaw(Shader shader)
{

    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    unsigned int emissiveNr = 0;
    
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string name;

        switch (textures[i].type)
        {
            default:
            case TypeTexture::DIFFUSE:
                name = "material.diffuse[" + std::to_string(diffuseNr++) + "]";
                break;
            case TypeTexture::SPECULAR:
                name = "material.specular[" + std::to_string(specularNr++) + "]";
                break;
            case TypeTexture::NORMAL:
                name = "material.normal[" + std::to_string(normalNr++) + "]";
                break;
            case TypeTexture::HEIGHT:
                name = "material.height[" + std::to_string(heightNr++) + "]";
                break;
            case TypeTexture::EMISSIVE:
                name = "material.emissive[" + std::to_string(emissiveNr++) + "]";
                break;
        }

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);

        shader.use();
        shader.setInt((name).c_str(), i);
    }

    shader.setInt("material.num_diffuse", diffuseNr);
    shader.setInt("material.num_specular", specularNr);
    shader.setInt("material.num_normal", normalNr);
    shader.setInt("material.num_height", heightNr);
    shader.setInt("material.num_emissive", emissiveNr);
    shader.setFloat("material.shininess", shininess);

    shader.use();
    // draw mesh
    glBindVertexArray(VAO);
    //glPolygonMode(GL_FRONT, GL_FILL);
    //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
