#include "Collider.h"

Collider::Collider()
{
    type = ColliderType::BOX;
    transform = new Transform();
    matHandle.type = MaterialType::LIT;
    Shader *colliderShader = new Shader("shaders/colliderShader.vert","shaders/colliderShader.frag");
    matHandle.EditMaterial(MaterialComponent::SHADER1, colliderShader);
    matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);
}

Collider::Collider(ColliderType typeCollider)
{
    type = typeCollider;
    transform = new Transform();
    matHandle.type = MaterialType::LIT;
    Shader* colliderShader = new Shader("shaders/colliderShader.vert", "shaders/colliderShader.frag");
    matHandle.EditMaterial(MaterialComponent::SHADER1, colliderShader);
    matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);

    if (type == ColliderType::MESH)
    {
        LoadMesh("");
    }
    else if (type == ColliderType::SPHERE)
    {
        LoadMesh("./resources/3DModels/sphere/sphere.obj");
    }
    else
    {
        std::vector<Vertex> verticesVertex;
        std::vector<unsigned int> indices;

        float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

            //BIEN
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

            //BIEN
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            //BIEN
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
             //SUPERIOR
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
            //INFERIOR
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
        };

        unsigned int NUMCOMP = 8;
        for (int i = 0; i < 36; i++)
        {
            unsigned int index = i * NUMCOMP;

            Vertex vertex;
            glm::vec3 vector;

            vector.x = vertices[index];
            vector.y = vertices[index + 1];
            vector.z = vertices[index + 2];
            vertex.Position = vector;

            vector.x = vertices[index + 3];
            vector.y = vertices[index + 4];
            vector.z = vertices[index + 5];
            vertex.Normal = vector;

            glm::vec2 vec;
            vec.x = vertices[index + 6];
            vec.y = vertices[index + 7];
            vertex.TexCoords = vec;

            verticesVertex.push_back(vertex);
            indices.push_back(i);
        }

        meshCollider.MeshCollider(verticesVertex, indices, matHandle);
    }
}

void Collider::DrawCollider()
{
    matHandle.shader->use();
    meshCollider.material->ptrShader->setMat4("model", transform->finalModelMatrix);

    glBindVertexArray(meshCollider.GetVAO());
    glDrawElements(GL_LINES, meshCollider.indices.size(), GL_UNSIGNED_INT, 0);
}

void Collider::LoadMesh(std::string path)
{
    Assimp::Importer importer;
    scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "ERROR::ASSIMP::%s", importer.GetErrorString());
        return;
    }

    recursiveNodeProcess(scene->mRootNode);

    processNode(scene->mRootNode, scene);
}

void Collider::recursiveNodeProcess(aiNode* node)
{
    ai_nodes.push_back(node);

    for (int i = 0; i < node->mNumChildren; i++)
        recursiveNodeProcess(node->mChildren[i]);
}

void Collider::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void Collider::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    meshCollider.MeshCollider(vertices, indices, matHandle);
}
