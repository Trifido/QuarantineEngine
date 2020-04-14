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

void Mesh::DrawOutline(Shader shIn, Shader shOutline)
{
    //STENCIL TEST & DEPTH TEST
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glStencilMask(0x00);
    //Stencil function
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    this->Draw(shIn);

    //Second Render PASS
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    this->Draw(shOutline);

    glStencilMask(0xFF);
    //glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}

bool Mesh::RayIntersectsTriangle(glm::vec3 origin, glm::vec3 dir, unsigned int idTri, float& intersectionDist, glm::mat4 model)
{
    const float EPSILON = 0.0000001;
    glm::vec3 edge1 = vertices[idTri + 1].Position - vertices[idTri].Position;
    glm::vec3 edge2 = vertices[idTri + 2].Position - vertices[idTri].Position;

    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.

    float f = 1.0 / a;
    glm::vec3 s = origin - vertices[idTri].Position;
    float u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0 || (u + v) > 1.0)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        glm::vec3 realOrigin = glm::vec4(origin, 1.0f) * model;
        glm::vec3 realDir = glm::vec4(dir, 1.0f) * model;
        intersectionDist = glm::distance(realOrigin, (realOrigin + realDir * t));
        //intersectionDist = glm::distance(origin, (origin + dir * t));
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}
