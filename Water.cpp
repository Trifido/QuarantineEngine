#include "Water.h"

Water::Water()
{
    float waterPlane[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    //Texture river_pbrDiff("./resources/cave/WATER/Water_001_COLOR.jpg", TypeTexture::DIFFUSE);
    //Texture river_pbrNorm("./resources/cave/WATER/Water_001_NORM.jpg", TypeTexture::NORMAL);
    //Texture river_pbrRO("./resources/cave/WATER/Water_001_SPEC.jpg", TypeTexture::ROUGHNESS);
    //Texture river_pbrAO("./resources/cave/WATER/Water_001_OCC.jpg", TypeTexture::AO);
    //Texture river_pbrHE("./resources/cave/WATER/Water_001_DISP.png", TypeTexture::HEIGHT);
    //textImages.push_back(river_pbrDiff);
    //textImages.push_back(river_pbrNorm);
    //textImages.push_back(river_pbrRO);
    //textImages.push_back(river_pbrAO);
    //textImages.push_back(river_pbrHE);

    distortionUV = new Texture("./resources/cave/WATER/flowmap2.png");
    normalUV = new Texture("./resources/cave/WATER/waterNormal.png");
    waterShader = new Shader("shaders/waterShader.vert", "shaders/waterShader.frag");
    transform = new Transform();
    tilingValue = 1;
    waveSpeed = 0.03f;
    transform->model = glm::translate(transform->model, glm::vec3(-12.210f, -0.5f, 35.970f));
    transform->model = glm::scale(transform->model, glm::vec3(3.210f, 1.0f, 1.740f));
    AddPlane(waterPlane, 6);
}

Water::Water(std::string path)
{
    waterShader = new Shader("shaders/waterShader.vert", "shaders/waterShader.frag");
    transform = new Transform();
    loadModel(path);
}

void Water::Render(std::vector<Light*> lights, unsigned int reflectTexture, unsigned int refractTexture)
{
    int numDir, numPoint, numSpot;
    numDir = numPoint = numSpot = 0;

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes.at(i).material->ptrShader->use();
        for (unsigned int idLight = 0; idLight < lights.size(); idLight++)
        {
            if (lights.at(idLight)->GetType() == TypeLight::DIRL)
            {
                std::string numLight = std::to_string(numDir);
                meshes[i].material->ptrShader->setMat4("DirlightSpaceMatrix[" + numLight + "]", lights.at(idLight)->lightSpaceMatrix);
                numDir++;
            }
            else if (lights.at(idLight)->GetType() == TypeLight::POINTLIGHT)
            {
                std::string numLight = std::to_string(numPoint);
                meshes[i].material->ptrShader->setMat4("PointlightSpaceMatrix[" + numLight + "]", lights.at(idLight)->lightSpaceMatrix);
                numPoint++;
            }
            else if (lights.at(idLight)->GetType() == TypeLight::SPOTL)
            {
                std::string numLight = std::to_string(numSpot);
                meshes[i].material->ptrShader->setMat4("SpotlightSpaceMatrix[" + numLight + "]", lights.at(idLight)->lightSpaceMatrix);
                numSpot++;
            }
        }

        meshes[i].material->ptrShader->setMat4("model", transform->finalModelMatrix);
        meshes[i].material->ptrShader->setInt("material.reflectionTexture", 0);
        meshes[i].material->ptrShader->setInt("material.refractionTexture", 1);
        meshes[i].material->ptrShader->setInt("material.distortionTexture", 2);
        meshes[i].material->ptrShader->setInt("material.normalTexture", 3);
        meshes[i].material->ptrShader->setFloat("tiling", tilingValue);
        meshes[i].material->SetNumTextures(4);

        moveFactor += waveSpeed * *deltaTime;

        if (moveFactor > 1.0f)
        {
            int entero = (int)moveFactor;
            moveFactor = moveFactor - entero;
        }

        meshes[i].material->ptrShader->setFloat("moveFactor", moveFactor);
        meshes[i].material->ptrShader->setVec3("waterColor", waterColor);
        meshes[i].material->ptrShader->setFloat("colorFactor", colorFactor);
        meshes[i].material->ptrShader->setFloat("factor_refractive", factor_refractive);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, reflectTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, refractTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, distortionUV->ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, normalUV->ID);

        meshes.at(i).material->ptrShader->ActivateCamera();
        meshes.at(i).material->ptrShader->ActivateLights();

        meshes[i].Draw();
    }
}

void Water::AddPlane(float rawData[], unsigned int numVertex)
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

    for (unsigned int idTr = 0; idTr < numVertex; idTr += 3)
    {
        glm::vec3 edge1 = vertices[idTr + 1].Position - vertices[idTr].Position;
        glm::vec3 edge2 = vertices[idTr + 2].Position - vertices[idTr].Position;
        glm::vec2 deltaUV1 = vertices[idTr + 1].TexCoords - vertices[idTr].TexCoords;
        glm::vec2 deltaUV2 = vertices[idTr + 2].TexCoords - vertices[idTr].TexCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent, bitangent;

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
    meshes.push_back(Mesh(vertices, indices, textImages, waterShader));
}

void Water::loadModel(std::string path)
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

void Water::processNode(aiNode* node, const aiScene* scene)
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

Mesh Water::processMesh(aiMesh* mesh, const aiScene* scene)
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
    return Mesh(vertices, indices, textImages, waterShader);
}

std::vector<Texture> Water::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName)
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

void Water::recursiveNodeProcess(aiNode* node)
{
    ai_nodes.push_back(node);

    for (int i = 0; i < node->mNumChildren; i++)
        recursiveNodeProcess(node->mChildren[i]);
}

void Water::AddCamera(Camera* mainCamera)
{
    waterShader->AddCamera(mainCamera);
}

void Water::AddLights(std::vector<Light*> lights)
{
    waterShader->AddLight(lights);
}

void Water::SetModelHierarchy()
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

void Water::ActivateIrradianceMap(unsigned int idTexIrradiance, unsigned int idTexPrefilter, unsigned int idTexBrdf)
{
    waterShader->use();
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes.at(i).material->ActivateIrradianceMap(idTexIrradiance, idTexPrefilter, idTexBrdf);
    }
}

void Water::ActivateShadowMap(unsigned int idTexShadow, unsigned int idLight, TypeLight type)
{
    waterShader->use();
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes.at(i).material->ActivateShadowTexture(idTexShadow, idLight, type);
    }
}

void Water::DeleteGPUInfo()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes.at(i).DelteGPUInfo();
}
