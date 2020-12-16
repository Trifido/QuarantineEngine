#include "AnimatedMesh.h"

AnimatedMesh::AnimatedMesh(std::vector<VertexAnimated> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, MaterialHandle& matHandle)
{
    this->vertices = vertices;
    this->indices = indices;
    //this->DetermineAdjacency();
    material = new Material(matHandle.shader, matHandle.shader2);
    material->ptrShaderShadow = matHandle.shaderShadow;
    material->ptrShaderPointShadow = matHandle.shaderPointShadow;
    material->AddMultTextures(textures);
    matHandle.AddMaterialToList(material);

    setupMesh();
}

void AnimatedMesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexAnimated), &vertices[0], GL_STATIC_DRAW);

    if (indices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnimated), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnimated), (void*)offsetof(VertexAnimated, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAnimated), (void*)offsetof(VertexAnimated, TexCoords));
    // vertex tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnimated), (void*)offsetof(VertexAnimated, Tangents));
    // vertex bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnimated), (void*)offsetof(VertexAnimated, Bitangents));
    //vertex weight
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexAnimated), (void*)offsetof(VertexAnimated, Weight));
    //vertex ID weight
    glEnableVertexAttribArray(6);
    glVertexAttribIPointer(6, 4, GL_INT, sizeof(VertexAnimated), (void*)offsetof(VertexAnimated, Id));

    glBindVertexArray(0);
}

void AnimatedMesh::Draw()
{
    material->AssignRenderTextures();
    SetRenderMode();
}

void AnimatedMesh::SetRenderMode()
{
    glBindVertexArray(VAO);

    switch (material->drawtype)
    {
    case DrawMode::DPOINTS:
        if (material->type != MaterialType::INSTANCE)
            glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArraysInstanced(GL_POINTS, 0, 6, 100);
        break;
    case DrawMode::DLINES:
        if (material->type != MaterialType::INSTANCE)
            glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArraysInstanced(GL_LINES, 0, 6, 100);
        break;
    case DrawMode::DLINES_STRIP:
        if (material->type != MaterialType::INSTANCE)
            glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawArraysInstanced(GL_LINE_STRIP, 0, 6, 100);
        break;
    case DrawMode::DTRIANGLES:
        if (isTriangleIndexSystem)
        {
            if (material->type != MaterialType::INSTANCE)
                glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            else
                glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, material->numInstances);
        }
        else
        {
            if (material->type != MaterialType::INSTANCE)
                glDrawElements(GL_TRIANGLES_ADJACENCY, stripTrianglesIndices.size(), GL_UNSIGNED_INT, 0);
            else
                glDrawElementsInstanced(GL_TRIANGLES_ADJACENCY, stripTrianglesIndices.size(), GL_UNSIGNED_INT, 0, material->numInstances);
        }
        break;
    case DrawMode::DTRIANGLES_STRIP:
        if (material->type != MaterialType::INSTANCE)
            glDrawElements(GL_TRIANGLES_ADJACENCY, stripTrianglesIndices.size(), GL_UNSIGNED_INT, 0);
        else
            glDrawElementsInstanced(GL_TRIANGLES_ADJACENCY, stripTrianglesIndices.size(), GL_UNSIGNED_INT, 0, material->numInstances);
        break;
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void AnimatedMesh::ChangeIndexSystem(bool isCommonIndex)
{
    glBindVertexArray(VAO);

    if (isCommonIndex != isTriangleIndexSystem)
    {
        if (isCommonIndex == true)
        {
            if (indices.size() > 0)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
            }
        }
        else
        {
            if (stripTrianglesIndices.size() > 0)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, stripTrianglesIndices.size() * sizeof(unsigned int), &stripTrianglesIndices[0], GL_STATIC_DRAW);
            }
        }
    }
    glBindVertexArray(0);

    isTriangleIndexSystem = isCommonIndex;
}
