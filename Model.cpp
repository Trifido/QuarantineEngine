#include "Model.h"

Model::Model()
{
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

Model::Model(char* path)
{
    loadModel(path);
}

void Model::Draw(Shader shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::DrawRaw(Shader shader)
{
    meshes[0].Draw(shader);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "ERROR::ASSIMP::%s",importer.GetErrorString());
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
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

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    existTangent = mesh->HasTangentsAndBitangents();
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        if (!existTangent)
        {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangents = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Tangents = vector;
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
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TypeTexture::SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TypeTexture::NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, TypeTexture::HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        //AÑADIR MAS TIPOS DE TEXTURA
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
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
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

void Model::Rotation(float radians, glm::vec3 axis)
{
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(radians), axis);
}

void Model::TranslationTo(glm::vec3 position)
{
    this->model = glm::translate(this->model, position);
}

void Model::ScaleTo(glm::vec3 scale)
{
    this->model = glm::scale(this->model, scale);
}

void Model::ResetModel()
{
    this->model = glm::mat4(1.0f);
}

void Model::DeleteGPUInfo()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes.at(i).DelteGPUInfo();
}

void Model::SetShininess(float shini)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes.at(i).SetShininess(shini);
}

Model::Model(float rawData[], unsigned int numVertex, std::vector<Texture> textImages)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const int NUMCOMP = 8;
    for (int i = 0; i < numVertex; i++)
    {
        unsigned int index = i * NUMCOMP;

        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = rawData[index];
        vector.y = rawData[index + 1];
        vector.z = rawData[index + 2];
        vertex.Position = vector;

        vector.x = rawData[index + 3];
        vector.y = rawData[index + 4];
        vector.z = rawData[index + 5];
        vertex.Normal = vector;

        glm::vec2 vec;
        vec.x = rawData[index + 6];
        vec.y = rawData[index + 7];
        vertex.TexCoords = vec;

        vertices.push_back(vertex);
        indices.push_back(i);
    }
    //Texturas
    textures_loaded = textImages;
    //Compute Tangent & BiTangents

    bool isNormal = false; 
    //Check Normal
    for (int i = 0; i < textures_loaded.size(); i++)
        if (textures_loaded.at(i).type == TypeTexture::NORMAL)
            isNormal = true;

    if (isNormal)
    {
        for (unsigned int idTr = 0; idTr < numVertex; idTr += 3)
        {
            glm::vec3 edge1 = vertices[idTr + 1].Position - vertices[idTr].Position;
            glm::vec3 edge2 = vertices[idTr + 2].Position - vertices[idTr].Position;
            glm::vec2 deltaUV1 = vertices[idTr + 1].TexCoords - vertices[idTr].TexCoords;
            glm::vec2 deltaUV2 = vertices[idTr + 2].TexCoords - vertices[idTr].TexCoords;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            glm::vec3 tangent, bitangent;

            if (!existTangent)
            {
                tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                tangent = glm::normalize(tangent);

                vertices[idTr].Tangents = tangent;
                vertices[idTr + 1].Tangents = tangent;
                vertices[idTr + 2].Tangents = tangent;

                bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                bitangent = glm::normalize(bitangent);

                vertices[idTr].Bitangents = bitangent;
                vertices[idTr + 1].Bitangents = bitangent;
                vertices[idTr + 2].Bitangents = bitangent;
            }
        }
    }

    meshes.push_back(Mesh(vertices, indices, textures_loaded));
}
