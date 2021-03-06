#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Shader* shader)
{ 
    this->vertices = vertices;
    this->indices = indices;
    this->DetermineAdjacency();
    this->material = new Material(shader);
    this->material->AddMultTextures(textures);
    setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, MaterialHandle& matHandle)
{ 
    this->vertices = vertices;
    this->indices = indices;
    this->DetermineAdjacency();
    material = new Material(matHandle.shader, matHandle.shader2);
    material->ptrShaderShadow = matHandle.shaderShadow;
    material->ptrShaderPointShadow = matHandle.shaderPointShadow; 
    matHandle.AddMaterialToList(material);

    setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, MaterialHandle& matHandle)
{
    this->vertices = vertices;
    this->indices = indices;
    this->DetermineAdjacency();
    material=new Material(matHandle.shader, matHandle.shader2);
    material->ptrShaderShadow = matHandle.shaderShadow;
    material->ptrShaderPointShadow = matHandle.shaderPointShadow;
    material->AddMultTextures(textures);
    matHandle.AddMaterialToList(material);

    setupMesh();
}

Mesh::Mesh(std::vector<ProceduralVertex> vertices, std::vector<unsigned int> indices, MaterialHandle& matHandle, glm::vec2* instances)
{
    this->proceduralVertices = vertices;
    this->indices = indices;
    this->DetermineAdjacency();
    material = new Material(matHandle.shader);
    material->ptrShaderShadow = matHandle.shaderShadow;
    material->ptrShaderPointShadow = matHandle.shaderPointShadow;
    matHandle.AddMaterialToList(material);

    setupProceduralMesh(instances);
}

void Mesh::setupProceduralMesh(glm::vec2* instances)
{
    // store instance data in an array buffer
// --------------------------------------
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, instances, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, proceduralVertices.size() * sizeof(ProceduralVertex), &proceduralVertices[0], GL_STATIC_DRAW);

    if (indices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ProceduralVertex), (void*)0);
    // vertex color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ProceduralVertex), (void*)(2 * sizeof(float)));
    //// vertex normals
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    //// vertex texture coords
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    //// vertex tangents
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangents));
    //// vertex bitangents
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangents));

    setUpInstanceMesh(instanceVBO);

    //glBindVertexArray(0);
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
    ////vertex weight
    //glEnableVertexAttribArray(5);
    //glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weight));
    ////vertex weight
    //glEnableVertexAttribArray(6);
    //glVertexAttribPointer(6, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Id));

    glBindVertexArray(0);
}

void Mesh::setupVolumetricMesh()
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
    
    glBindVertexArray(0);
}

void Mesh::setUpInstanceMesh(unsigned int ID)
{
    //set instance data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, ID); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
}

void Mesh::MeshCollider(std::vector<Vertex> vertices, std::vector<unsigned int> indices, MaterialHandle& matHandle)
{
    this->vertices = vertices;
    this->indices = indices;
    material = new Material(matHandle.shader, matHandle.shader2);
    matHandle.AddMaterialToList(material);

    setupVolumetricMesh();
    //setupMesh();
}

void Mesh::ScaleMeshCollider(glm::vec3 scalevert)
{
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        if(scalevert.x != 1.0f)
            vertices.at(i).Position.x *= scalevert.x;
        if (scalevert.y != 1.0f)
            vertices.at(i).Position.y *= scalevert.y;
        if (scalevert.z != 1.0f)
            vertices.at(i).Position.z *= scalevert.z;
    }

    setupMesh();
}

void Mesh::Draw(bool isOutline, bool isActive)
{
    if (!isOutline || !isActive)
    {
        material->AssignRenderTextures();

        SetRenderMode();

        if (material->type == MaterialType::NORMALS)
        {
            material->ptrShader2->use();
            material->ptrShader2->ActivateCamera();
            SetRenderMode();
        }
    }
    else if (isActive)
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

        material->AssignRenderTextures();

        SetRenderMode();

        //Second Render PASS
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        material->ptrShader2->use();
        material->ptrShader2->ActivateCamera();

        SetRenderMode();

        glStencilMask(0xFF);
        //glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    }
}

void Mesh::DrawShadow()
{
    SetRenderMode();
}

void Mesh::DrawVolumeShadow()
{
    glBindVertexArray(VAO);

    if (material->type != MaterialType::INSTANCE)
        glDrawElements(GL_TRIANGLES_ADJACENCY, stripTrianglesIndices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawElementsInstanced(GL_TRIANGLES_ADJACENCY, stripTrianglesIndices.size(), GL_UNSIGNED_INT, 0, material->numInstances);

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawOcclusionScattering(Shader* sh)
{
    material->AssignRenderTextures(sh);
    SetRenderMode();
}

void Mesh::DelteGPUInfo()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, MaterialHandle& matHandle)
{
    this->vertices = vertices;
    material = new Material(matHandle.shader);
    material->AddMultTextures(textures);
    matHandle.AddMaterialToList(material);
    setupMesh();
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

    //this->Draw(shIn);
    this->Draw();

    //Second Render PASS
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    //this->Draw(shOutline);
    this->Draw();

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

void Mesh::SetRenderMode()
{
    glBindVertexArray(VAO);

    switch (material->drawtype)
    {
    case DrawMode::DPOINTS:
        if(material->type != MaterialType::INSTANCE)
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

void Mesh::SetIntanceMesh()
{

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
    // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    glBindVertexArray(VAO);
    // set attribute pointers for matrix (4 times vec4)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}

void Mesh::SetBoundingLight(bool value)
{
    isBoundingLight = value;
    material->isBounding = value;
}

void Mesh::ChangeIndexSystem(bool isCommonIndex)
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

void Mesh::DetermineAdjacency()
{
    // Copy and make room for adjacency info
    for (GLuint i = 0; i < indices.size(); i += 3)
    {
        stripTrianglesIndices.push_back(indices[i]);
        stripTrianglesIndices.push_back(-1);
        stripTrianglesIndices.push_back(indices[i + 1]);
        stripTrianglesIndices.push_back(-1);
        stripTrianglesIndices.push_back(indices[i + 2]);
        stripTrianglesIndices.push_back(-1);
    }

    // Find matching edges
    for (GLuint i = 0; i < stripTrianglesIndices.size(); i += 6)
    {
        // A triangle
        int a1 = stripTrianglesIndices[i];
        int b1 = stripTrianglesIndices[i + 2];
        int c1 = stripTrianglesIndices[i + 4];

        // Scan subsequent triangles
        for (GLuint j = i + 6; j < stripTrianglesIndices.size(); j += 6)
        {
            int a2 = stripTrianglesIndices[j];
            int b2 = stripTrianglesIndices[j + 2];
            int c2 = stripTrianglesIndices[j + 4];

            // Edge 1 == Edge 1
            if ((a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2))
            {
                stripTrianglesIndices[i + 1] = c2;
                stripTrianglesIndices[j + 1] = c1;
            }
            // Edge 1 == Edge 2
            if ((a1 == b2 && b1 == c2) || (a1 == c2 && b1 == b2))
            {
                stripTrianglesIndices[i + 1] = a2;
                stripTrianglesIndices[j + 3] = c1;
            }
            // Edge 1 == Edge 3
            if ((a1 == c2 && b1 == a2) || (a1 == a2 && b1 == c2))
            {
                stripTrianglesIndices[i + 1] = b2;
                stripTrianglesIndices[j + 5] = c1;
            }
            // Edge 2 == Edge 1
            if ((b1 == a2 && c1 == b2) || (b1 == b2 && c1 == a2))
            {
                stripTrianglesIndices[i + 3] = c2;
                stripTrianglesIndices[j + 1] = a1;
            }
            // Edge 2 == Edge 2
            if ((b1 == b2 && c1 == c2) || (b1 == c2 && c1 == b2))
            {
                stripTrianglesIndices[i + 3] = a2;
                stripTrianglesIndices[j + 3] = a1;
            }
            // Edge 2 == Edge 3
            if ((b1 == c2 && c1 == a2) || (b1 == a2 && c1 == c2))
            {
                stripTrianglesIndices[i + 3] = b2;
                stripTrianglesIndices[j + 5] = a1;
            }
            // Edge 3 == Edge 1
            if ((c1 == a2 && a1 == b2) || (c1 == b2 && a1 == a2))
            {
                stripTrianglesIndices[i + 5] = c2;
                stripTrianglesIndices[j + 1] = b1;
            }
            // Edge 3 == Edge 2
            if ((c1 == b2 && a1 == c2) || (c1 == c2 && a1 == b2))
            {
                stripTrianglesIndices[i + 5] = a2;
                stripTrianglesIndices[j + 3] = b1;
            }
            // Edge 3 == Edge 3
            if ((c1 == c2 && a1 == a2) || (c1 == a2 && a1 == c2))
            {
                stripTrianglesIndices[i + 5] = b2;
                stripTrianglesIndices[j + 5] = b1;
            }
        }
    }

    // Look for any outside edges
    for (GLuint i = 0; i < stripTrianglesIndices.size(); i += 6)
    {
        if (stripTrianglesIndices[i + 1] == -1) stripTrianglesIndices[i + 1] = stripTrianglesIndices[i + 4];
        if (stripTrianglesIndices[i + 3] == -1) stripTrianglesIndices[i + 3] = stripTrianglesIndices[i];
        if (stripTrianglesIndices[i + 5] == -1) stripTrianglesIndices[i + 5] = stripTrianglesIndices[i + 2];
    }

    //indices = stripTrianglesIndices;
}
