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
        radius = 1.0f;
    }
    else
    {
        std::vector<Vertex> verticesVertex;
        std::vector<unsigned int> indicesV;

        float verticesV[] = {
            // positions         
            -0.5f, -0.5f, -0.5f,// 0
             0.5f, -0.5f, -0.5f,// 1
             0.5f,  0.5f, -0.5f,// 2
            -0.5f,  0.5f, -0.5f,// 3
             
            -0.5f, -0.5f, 0.5f, // 4
             0.5f, -0.5f, 0.5f, // 5
             0.5f,  0.5f, 0.5f, // 6
            -0.5f,  0.5f, 0.5f  // 7
        };

        unsigned int COUNT = 8;
        for (int i = 0; i < COUNT; i++)
        {
            unsigned int index = i * 3;

            Vertex vertex;
            glm::vec3 vector;

            vector.x = verticesV[index];
            vector.y = verticesV[index + 1];
            vector.z = verticesV[index + 2];
            vertex.Position = vector;

            verticesVertex.push_back(vertex);
        }

        indicesV.push_back(0);
        indicesV.push_back(1);
        indicesV.push_back(2);
        indicesV.push_back(3);
        indicesV.push_back(0);
        indicesV.push_back(4);
        indicesV.push_back(5);
        indicesV.push_back(1);
        indicesV.push_back(2);
        indicesV.push_back(6);
        indicesV.push_back(5);
        indicesV.push_back(4);
        indicesV.push_back(7);
        indicesV.push_back(6);
        indicesV.push_back(2);
        indicesV.push_back(3);
        indicesV.push_back(7);
        indicesV.push_back(4);
        indicesV.push_back(0);
        indicesV.push_back(3);

        meshCollider.MeshCollider(verticesVertex, indicesV, matHandle);
    }
}

void Collider::DrawCollider()
{
    SetHierarchy();
    matHandle.shader->use();
    meshCollider.material->ptrShader->setMat4("model", transform->finalModelMatrix);

    glBindVertexArray(meshCollider.GetVAO());

    switch (type)
    {
    case ColliderType::BOX:
        glDrawElements(GL_LINE_STRIP, meshCollider.indices.size(), GL_UNSIGNED_INT, 0);
        break;
    case ColliderType::SPHERE:
    case ColliderType::MESH:
        glDrawElements(GL_LINES, meshCollider.indices.size(), GL_UNSIGNED_INT, 0);
        break;
    }
    
}

bool Collider::IsRayCollision(UIRay* ray)
{
    float distRay = FLT_MAX;
    glm::vec4 ray_eye = inverse(ray->PV * transform->finalModelMatrix) * glm::vec4(ray->ray_dir, 1.0);
    glm::vec3 ray_wor = glm::normalize(ray_eye);
    glm::vec3 ray_orig = inverse(transform->finalModelMatrix) * glm::vec4(ray->ray_orig, 1.0f);

    if (type == ColliderType::BOX)
    {
        float dist = CheckCollider(ray_orig, ray_wor);
        if (dist > 0.0f && dist < distRay)
        {
            return true;
        }
    }

    return false;
}

float Collider::CheckCollider(glm::vec3 origin, glm::vec3 dir)
{
    float intersectionDist;
    float distancePoint = FLT_MAX;

    switch (type)
    {
        case ColliderType::MESH:
            for (unsigned idTri = 0; idTri < meshCollider.indices.size(); idTri += 3)
            {
                if (meshCollider.RayIntersectsTriangle(origin, dir, idTri, intersectionDist, transform->model))
                {
                    if (distancePoint > intersectionDist)
                        distancePoint = intersectionDist;
                }
            }
            break;
        case ColliderType::SPHERE:
            break;
        case ColliderType::BOX:
            break;
    }

    

    return distancePoint;
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

void Collider::SetHierarchy()
{
    transform->finalModelMatrix = transform->model;

    if (transform->parent != nullptr)
    {
        Transform* parent = transform->parent;

        while (parent != nullptr)
        {
            transform->finalModelMatrix *= parent->model;
            parent = parent->parent;
        }
    }
}
