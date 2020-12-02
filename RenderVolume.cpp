#include "RenderVolume.h"

RenderVolume::RenderVolume()
{
    type = RenderVolumeType::BOX_VOL;
    aabb_min = glm::vec3(-1.0f);
    aabb_max = glm::vec3(1.0f);
    radius = 1.0f;
    transform = new Transform();
    matHandle.type = MaterialType::LIT;
    Shader* renderVolumeShader = new Shader("shaders/colliderShader.vert", "shaders/colliderShader.frag");
    matHandle.EditMaterial(MaterialComponent::SHADER1, renderVolumeShader);
    matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);
}

RenderVolume::RenderVolume(RenderVolumeType typeVolume, NoiseType type_procedural_texture)
{
    switch (type_procedural_texture)
    {
        case PERLIN:;
            procedural_textures.push_back(perlin = new ProceduralTexture(type_procedural_texture, 1024));
            break;
        case WORLEY:
            procedural_textures.push_back(worley =new ProceduralTexture(type_procedural_texture, 1024, 3, 20));
            break;
        case ALL:
        default:
            procedural_textures.push_back(perlin = new ProceduralTexture(type_procedural_texture, 1024));
            procedural_textures.push_back(worley = new ProceduralTexture(type_procedural_texture, 1024, 3, 20));
            break;
    }

    type = typeVolume;
    transform = new Transform();
    matHandle.type = MaterialType::LIT;
    Shader* colliderShader = new Shader("shaders/rmVolumeTest.vert", "shaders/rmVolumeTest.frag");
    matHandle.EditMaterial(MaterialComponent::SHADER1, colliderShader);
    matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);

    if (type == RenderVolumeType::MESH_VOL)
    {
        LoadMesh("");
    }
    else if (type == RenderVolumeType::SPHERE_VOL)
    {
        LoadMesh("./resources/3DModels/sphere/sphere.obj");
        radius = 1.0f;
    }
    else
    {
        aabb_min = glm::vec3(-1.0f);
        aabb_max = glm::vec3(1.0f);
        std::vector<Vertex> verticesVertex;
        std::vector<unsigned int> indicesV;

        float verticesV[] = {
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
        /*
        float verticesV[] = {
            // positions         
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,// 0
             1.0f, -1.0f, -1.0f, 1.0f, 0.0f,// 1
             1.0f,  1.0f, -1.0f, 1.0f, 1.0f,// 2
            -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,// 3

            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,// 4
             1.0f, -1.0f, 1.0f, 1.0f, 0.0f,// 5
             1.0f,  1.0f, 1.0f, 1.0f, 1.0f,// 6
            -1.0f,  1.0f, 1.0f,  0.0f, 1.0f// 7
        };
        */
        //unsigned int COUNT = 8;
        //for (int i = 0; i < COUNT; i++)
        //{
        //    unsigned int index = i * 5;

        //    Vertex vertex;
        //    glm::vec3 vector;
        //    glm::vec2 uvCoords;

        //    vector.x = verticesV[index];
        //    vector.y = verticesV[index + 1];
        //    vector.z = verticesV[index + 2];
        //    uvCoords.x = verticesV[index + 3];
        //    uvCoords.y = verticesV[index + 4];
        //    vertex.Position = vector;
        //    vertex.TexCoords = uvCoords;
        //    verticesVertex.push_back(vertex);
        //}

        unsigned int COUNT = 36;
        for (int i = 0; i < COUNT; i++)
        {
            unsigned int index = i * 8;

            Vertex vertex;
            glm::vec3 vector;
            glm::vec3 normal;
            glm::vec2 uvCoords;

            vector.x = verticesV[index];
            vector.y = verticesV[index + 1];
            vector.z = verticesV[index + 2];
            normal.x = verticesV[index + 3];
            normal.y = verticesV[index + 4];
            normal.z = verticesV[index + 5];
            uvCoords.x = verticesV[index + 6];
            uvCoords.y = verticesV[index + 7];
            vertex.Position = vector;
            vertex.TexCoords = uvCoords;
            verticesVertex.push_back(vertex);
        }
        /*
        indicesV.push_back(5);
        indicesV.push_back(6);
        indicesV.push_back(7);

        indicesV.push_back(4);
        indicesV.push_back(5);
        indicesV.push_back(7);

        indicesV.push_back(1);
        indicesV.push_back(5);
        indicesV.push_back(4);

        indicesV.push_back(4);
        indicesV.push_back(0);
        indicesV.push_back(1);

        indicesV.push_back(6);
        indicesV.push_back(5);
        indicesV.push_back(1);

        indicesV.push_back(2);
        indicesV.push_back(6);
        indicesV.push_back(1);

        indicesV.push_back(2);
        indicesV.push_back(1);
        indicesV.push_back(0);

        indicesV.push_back(3);
        indicesV.push_back(2);
        indicesV.push_back(0);

        indicesV.push_back(4);
        indicesV.push_back(3);
        indicesV.push_back(0);

        indicesV.push_back(7);
        indicesV.push_back(3);
        indicesV.push_back(4);

        indicesV.push_back(6);
        indicesV.push_back(2);
        indicesV.push_back(3);

        indicesV.push_back(7);
        indicesV.push_back(6);
        indicesV.push_back(3);
        */

        for (unsigned int i = 0; i < 36; i++)
            indicesV.push_back(i);

        meshCollider.MeshCollider(verticesVertex, indicesV, matHandle);
    }
}

void RenderVolume::DrawRenderVolume()
{
    SetHierarchy();
    matHandle.shader->use();
    meshCollider.material->ptrShader->setMat4("model", transform->finalModelMatrix);
    meshCollider.material->ptrShader->setFloat("aspect_ratio", cam->WIDTH/cam->HEIGHT);
    meshCollider.material->ptrShader->setFloat("focal_length", 1.0f / tanf(M_PI / 180.0f * cam->GetFOV() / 2.0f));
    meshCollider.material->ptrShader->setVec2("viewport_size", glm::vec2(cam->WIDTH, cam->HEIGHT));
    meshCollider.material->ptrShader->setVec3("collider.top", glm::vec3(1.0f));
    meshCollider.material->ptrShader->setVec3("collider.bottom", glm::vec3(-1.0f));
    meshCollider.material->ptrShader->setFloat("time", glfwGetTime());

    int worleyNr, perlinNr;
    worleyNr = perlinNr = 0;

    for (unsigned int i = 0; i < procedural_textures.size(); i++)
    {
        std::string name;

        switch (procedural_textures[i]->typeNoise)
        {
        default:
        case NoiseType::PERLIN:
            name = "material.perlin[" + std::to_string(perlinNr++) + "]";
            break;
        case NoiseType::WORLEY:
            name = "material.worley[" + std::to_string(worleyNr++) + "]";
            break;
        }
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, procedural_textures[i]->ID);

        meshCollider.material->ptrShader->use();
        meshCollider.material->ptrShader->setInt((name).c_str(), i);
    }

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, perlin->ID);
    //meshCollider.material->ptrShader->use();
    //name = "material.perlin[" + std::to_string(perlinNr++) + "]";
    //meshCollider.material->ptrShader->setInt((name).c_str(), 0);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, worley->ID);
    //meshCollider.material->ptrShader->use();
    //name = "material.worley[" + std::to_string(worleyNr++) + "]";
    //meshCollider.material->ptrShader->setInt((name).c_str(), 1);

    meshCollider.material->ptrShader->setInt("material.num_worley", worleyNr);
    meshCollider.material->ptrShader->setInt("material.num_perlin", perlinNr);

    glBindVertexArray(meshCollider.GetVAO());

    switch (type)
    {
    case RenderVolumeType::BOX_VOL:
        glDrawElements(GL_TRIANGLES, meshCollider.indices.size(), GL_UNSIGNED_INT, 0);
        break;
    case RenderVolumeType::SPHERE_VOL:
    case RenderVolumeType::MESH_VOL:
        glDrawElements(GL_LINES, meshCollider.indices.size(), GL_UNSIGNED_INT, 0);
        break;
    }
}

void RenderVolume::AddCamera(Camera* cam)
{
    this->cam = cam;
    matHandle.AddCamera(cam);
}

void RenderVolume::LoadMesh(std::string path)
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

void RenderVolume::recursiveNodeProcess(aiNode* node)
{
    ai_nodes.push_back(node);

    for (int i = 0; i < node->mNumChildren; i++)
        recursiveNodeProcess(node->mChildren[i]);
}

void RenderVolume::processNode(aiNode* node, const aiScene* scene)
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

void RenderVolume::processMesh(aiMesh* mesh, const aiScene* scene)
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

void RenderVolume::SetHierarchy()
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
