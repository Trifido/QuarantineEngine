#include "GodRay.h"

void GodRay::loadModel(std::string path)
{
    Assimp::Importer importer;
    scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "ERROR::ASSIMP::%s", importer.GetErrorString());
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    recursiveNodeProcess(scene->mRootNode);
    processNode(scene->mRootNode, scene);
}

void GodRay::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh GodRay::processMesh(aiMesh* mesh, const aiScene* scene)
{
    unsigned int WEIGHTS_PER_VERTEX = 4;
    existTangent = mesh->HasTangentsAndBitangents();
    existNormal = mesh->HasNormals();
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

        if (existNormal)
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if (existTangent)
        {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangents = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangents = vector;
        }

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TypeTexture::DIFFUSE);
        textImages.insert(textImages.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TypeTexture::SPECULAR);
        textImages.insert(textImages.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TypeTexture::NORMAL);
        textImages.insert(textImages.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, TypeTexture::HEIGHT);
        textImages.insert(textImages.end(), heightMaps.begin(), heightMaps.end());
        // metallic maps
        std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType_METALNESS, TypeTexture::METALLIC);
        textImages.insert(textImages.end(), metallicMaps.begin(), metallicMaps.end());
        // AO maps
        std::vector<Texture> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TypeTexture::AO);
        textImages.insert(textImages.end(), aoMaps.begin(), aoMaps.end());
        // Roughness maps
        std::vector<Texture> roughMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, TypeTexture::ROUGHNESS);
        textImages.insert(textImages.end(), roughMaps.begin(), roughMaps.end());
        // Bump maps
        std::vector<Texture> bumpMaps;
        if (normalMaps.empty())
            bumpMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TypeTexture::NORMAL);
        else
            bumpMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TypeTexture::BUMP);
        textImages.insert(textImages.end(), bumpMaps.begin(), bumpMaps.end());
    }
    return Mesh(vertices, indices, textImages, godRayShader);
}

void GodRay::recursiveNodeProcess(aiNode* node)
{
    ai_nodes.push_back(node);

    for (int i = 0; i < node->mNumChildren; i++)
        recursiveNodeProcess(node->mChildren[i]);
}

std::vector<Texture> GodRay::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textImages.size(); j++)
        {
            if (std::strcmp(textImages[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textImages[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            std::string filename = std::string(str.C_Str());
            filename = directory + '/' + filename;

            Texture texture(filename);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textImages.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

void GodRay::SetModelHierarchy()
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

void GodRay::DeleteGPUInfo()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes.at(i).DelteGPUInfo();
}

GodRay::GodRay()
{
    godRayShader = new Shader("shaders/blending.vert", "shaders/blending.frag");
    transform = new Transform();
    loadModel("./resources/cave/GODRAY/godray.obj");

    godRayShader->setInt("colorTexture", 0);
}

void GodRay::Render()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes.at(i).material->ptrShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshes.at(i).material->textures.at(0).ID);
        meshes[i].material->ptrShader->setMat4("model", transform->finalModelMatrix);
        meshes.at(i).Draw();
    }
}

void GodRay::ScaleTo(glm::vec3 scale)
{
    transform->model = glm::scale(transform->model, scale);
}
