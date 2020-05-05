#include "Model.h"

Model::Model()
{
    model_type = ModelType::REGULAR_M;
    matHandle.type = MaterialType::LIT;
}

Model::Model(char* path)
{
    model_type = ModelType::REGULAR_M;
    matHandle.type = MaterialType::LIT;
    loadModel(path);
}

void Model::isSelectable(bool selectable) { this->isSelectableModel = selectable; }
void Model::isSelected(bool select) { this->isSelectedModel = select; }

void Model::SetCastShadow(bool value)
{
    CAST_SHADOW = value;
}

void Model::Draw(bool isOutline)
{
    if (!isOutline || !isSelectableModel || !isSelectedModel)
    {
        matHandle.shader->use();
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            if (matHandle.type != MaterialType::INSTANCE)
            {
                meshes[i].material->ptrShader->setMat4("model", model);
                if (meshes[i].material->type == MaterialType::NORMALS)
                    meshes[i].material->ptrShader2->setMat4("model", model);
            }
            meshes[i].Draw();
        }
    }
    else
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            matHandle.shader->use();
            meshes[i].material->ptrShader->setMat4("model", model);
            matHandle.shader2->use();
            meshes[i].material->ptrShader2->setMat4("model", glm::scale(model, glm::vec3(1.005f, 1.005f, 1.005f)));
            meshes[i].Draw(isOutline, isSelectedModel);
        }
    }
}

void Model::DrawCastShadow(Light* light, bool isOutline)
{
    if (!isOutline || !isSelectableModel || !isSelectedModel)
    {
        matHandle.shader->use();
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            if (matHandle.type != MaterialType::INSTANCE)
            {
                meshes[i].material->ptrShader->setMat4("lightSpaceMatrix", light->lightSpaceMatrix);
                meshes[i].material->ptrShader->setVec3("positionLight", light->GetPosition());
                meshes[i].material->ptrShader->setMat4("model", model);
                if (meshes[i].material->type == MaterialType::NORMALS)
                    meshes[i].material->ptrShader2->setMat4("model", model);
            }
            meshes[i].Draw();
        }
    }
    else
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            matHandle.shader->use();
            meshes[i].material->ptrShader->setMat4("lightSpaceMatrix", light->lightSpaceMatrix);
            meshes[i].material->ptrShader->setVec3("positionLight", light->GetPosition());
            meshes[i].material->ptrShader->setMat4("model", model);
            matHandle.shader2->use();
            meshes[i].material->ptrShader2->setMat4("model", glm::scale(model, glm::vec3(1.005f, 1.005f, 1.005f)));
            meshes[i].Draw(isOutline, isSelectedModel);
        }
    }
}

void Model::DrawShadow(glm::mat4 VPShadow)
{
    if (CAST_SHADOW)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i].material->ptrShaderShadow->use();
            meshes[i].material->ptrShaderShadow->setMat4("model", model);
            meshes[i].material->ptrShaderShadow->setMat4("lightSpaceMatrix", VPShadow);
            meshes[i].DrawShadow();
        }
    }
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

void Model::SetIntanceModelMatrix()
{
    modelMatrices = new glm::mat4[matHandle.MAX_INSTANCES];
    srand(glfwGetTime()); // initialize random seed	
    float radius = 150.0;
    float offset = 25.0f;
    for (unsigned int i = 0; i < matHandle.numInstances; i++)
    {
        glm::mat4 model_ins = this->model;
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)matHandle.numInstances * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model_ins = glm::translate(model_ins, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model_ins = glm::scale(model_ins, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model_ins = glm::rotate(model_ins, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model_ins;
    }

    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, matHandle.numInstances * sizeof(glm::mat4), &modelMatrices[0], GL_STREAM_DRAW);

    for (int i = 0; i < meshes.size(); i++)
    {
        meshes.at(i).SetIntanceMesh();
    }
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
    bool existNormal = mesh->HasNormals();
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
     
    return Mesh(vertices, indices, textures, matHandle);
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

void Model::SetInstanceModel(unsigned int ID)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes.at(i).setUpInstanceMesh(ID);
}

Model::Model(float rawData[], unsigned int numVertex, unsigned int offset, ModelType model_type, glm::vec2* instances)
{
    this->model_type = model_type;
    matHandle.type = MaterialType::LIT;

    if (model_type == ModelType::REGULAR_M)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned int NUMCOMP = offset;
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
        meshes.push_back(Mesh(vertices, indices, textures_loaded, matHandle));
    }
    else
    { 
        std::vector<ProceduralVertex> vertices;
        std::vector<unsigned int> indices;

        unsigned int NUMCOMP = offset;
        for (int i = 0; i < numVertex; i++)
        {
            unsigned int index = i * NUMCOMP;

            ProceduralVertex vertex;
            // process vertex positions, normals and texture coordinates
            glm::vec2 vector;
            glm::vec3 vectorColor;
            vector.x = rawData[index];
            vector.y = rawData[index + 1];
            vertex.Position = vector;

            vectorColor.x = rawData[index + 2];
            vectorColor.y = rawData[index + 3];
            vectorColor.z = rawData[index + 4];
            vertex.Color = vectorColor;

            vertices.push_back(vertex);
            indices.push_back(i);
        }

        meshes.push_back(Mesh(vertices, indices, matHandle, instances));
    }
}

Model::Model(float rawData[], unsigned int numVertex, std::vector<Texture> textImages)
{
    matHandle.type = MaterialType::LIT;

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
    meshes.push_back(Mesh(vertices, indices, textures_loaded, matHandle));
}

void Model::AddTextures(std::vector<Texture> texts)
{
    textures_loaded = texts;
    meshes.at(0).material->AddMultTextures(textures_loaded);
}

float Model::checkClickMouse(glm::vec3 origin, glm::vec3 dir)
{
    //Möller–Trumbore intersection algorithm
    if (!isSelectableModel)
        return INFINITE;

    bool existIntesection = false;
    float intersectionDist;
    float distancePoint = INFINITE;

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        for (unsigned idTri = 0; idTri < meshes[i].indices.size(); idTri += 3)
        {
            if (meshes[i].RayIntersectsTriangle(origin, dir, idTri, intersectionDist, model))
            {
                if (distancePoint > intersectionDist)
                    distancePoint = intersectionDist;
            }
        }
    }

    return distancePoint;
}

void Model::AddMaterial(Material* mat)
{
    matHandle.EditMaterial(MaterialComponent::TYPE, mat->type);
    matHandle.EditMaterial(MaterialComponent::SHADER1, mat->ptrShader);
    matHandle.EditMaterial(MaterialComponent::SHADER2, mat->ptrShader2);
    matHandle.EditMaterial(MaterialComponent::SHININESS, mat->shininess);
    matHandle.EditMaterial(MaterialComponent::OUTLINE_COLOR, mat->colorOutline);

    if (!mat->textures.empty())
    {
        matHandle.EditMaterial(MaterialComponent::TEXTURE, mat->textures);
    }
}

void Model::AddMaterial(Material* mat, std::vector<Texture> textures)
{
    matHandle.EditMaterial(MaterialComponent::TYPE, mat->type);
    matHandle.EditMaterial(MaterialComponent::SHADER1, mat->ptrShader);
    matHandle.EditMaterial(MaterialComponent::SHADER2, mat->ptrShader2);
    matHandle.EditMaterial(MaterialComponent::SHININESS, mat->shininess);
    matHandle.EditMaterial(MaterialComponent::OUTLINE_COLOR, mat->colorOutline);
    matHandle.EditMaterial(MaterialComponent::TEXTURE, textures);
}

void Model::AddShader(Shader* shader)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        matHandle.listMaterials.at(i)->ptrShader = shader;
        //(*matHandle.listMaterials.at(i)->shader) = shader;
        //meshes.at(i).material->shader = &this->shader;
    }
}

void Model::AddLight(std::vector<Light*> lights)
{
    matHandle.shader->AddLight(lights);
    if (matHandle.type == MaterialType::OUTLINE)
    {
        matHandle.shader2->AddLight(lights);
        //for (int i = 0; i < meshes.size(); i++)
        //{
        //    (*meshes.at(i).material->shader2)->lights = lights;
        //}
    }
}

void Model::AddCamera(Camera* cam)
{
    matHandle.shader->AddCamera(cam);
    if (matHandle.type == MaterialType::OUTLINE || matHandle.type == MaterialType::NORMALS)
    {
        matHandle.shader2->AddCamera(cam);

        //for (int i = 0; i < meshes.size(); i++)
        //    (*meshes.at(i).material->shader2)->mainCamera = cam;
    }
}
