#include "AnimatedModel.h"

void AnimatedModel::LoadModel(std::string path)
{
    scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    meshes = new std::vector<AnimatedMesh>();

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "ERROR::ASSIMP::%s", importer.GetErrorString());
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    recursiveNodeProcess(scene->mRootNode);
    AnimNodeProcess();
    globalInverseTransform = glm::inverse(AiToGLMMat4(scene->mRootNode->mTransformation));

    processNode(scene->mRootNode, scene);

    CreateBoneStructure();

    //Cargamos la animación
    //DoAnimation(animation);
}

void AnimatedModel::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes->push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

AnimatedMesh AnimatedModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
    unsigned int WEIGHTS_PER_VERTEX = 4;
    existTangent = mesh->HasTangentsAndBitangents();
    existNormal = mesh->HasNormals();
    std::vector<VertexAnimated> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        VertexAnimated vertex;
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
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TypeTexture::NORMAL);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, TypeTexture::HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        // metallic maps
        std::vector<Texture> metallicMaps = loadMaterialTextures(material, aiTextureType_METALNESS, TypeTexture::METALLIC);
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
        // AO maps
        std::vector<Texture> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TypeTexture::AO);
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
        // Roughness maps
        std::vector<Texture> roughMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, TypeTexture::ROUGHNESS);
        textures.insert(textures.end(), roughMaps.begin(), roughMaps.end());
        // Bump maps
        std::vector<Texture> bumpMaps;
        if (normalMaps.empty())
            bumpMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TypeTexture::NORMAL);
        else
            bumpMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TypeTexture::BUMP);
        textures.insert(textures.end(), bumpMaps.begin(), bumpMaps.end());
        //AÑADIR MAS TIPOS DE TEXTURA
    }

    ///---ANIMATION---
    std::vector<std::map<float, unsigned int> > vertexData;
    vertexData.resize(vertices.size());

    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        aiBone* aiBone = mesh->mBones[i];
        for (int j = 0; j < aiBone->mNumWeights; j++)
        {
            aiVertexWeight weight = aiBone->mWeights[j];
            vertexData.at(weight.mVertexId).insert(std::pair<float, unsigned int>(weight.mWeight, i));
        }
    }

    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        if (!vertexData.at(i).empty())
        {
            std::map<float, unsigned int>::iterator it = vertexData.at(i).end();
            --it;
            unsigned int k = 0;
            while (k < 4 && it != (--vertexData.at(i).begin()))
            {
                vertices.at(i).Id[k] = it->second;
                vertices.at(i).Weight[k] = it->first;
                it--;
                k++;
            }
        }
    }
     
    return AnimatedMesh(vertices, indices, textures, matHandle);
}

std::vector<Texture> AnimatedModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TypeTexture typeName)
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

void AnimatedModel::recursiveNodeProcess(aiNode* node)
{
    ai_nodes.push_back(node);

    for (int i = 0; i < node->mNumChildren; i++)
        recursiveNodeProcess(node->mChildren[i]);
}

void AnimatedModel::AnimNodeProcess()
{
    if (scene->mNumAnimations == 0)
        return;

    for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
        ai_nodes_anim.push_back(scene->mAnimations[0]->mChannels[i]);
}

Bone* AnimatedModel::FindBone(std::string name)
{
    for (int i = 0; i < bones.size(); i++)
    {
        if (bones.at(i).name == name)
            return &bones.at(i);
    }
    return nullptr;
}

aiNode* AnimatedModel::FindAiNode(std::string name)
{
    for (int i = 0; i < ai_nodes.size(); i++)
    {
        if (ai_nodes.at(i)->mName.data == name)
            return ai_nodes.at(i);
    }
    return nullptr;
}

aiNodeAnim* AnimatedModel::FindAiNodeAnim(std::string name)
{
    for (int i = 0; i < ai_nodes_anim.size(); i++)
    {
        if (ai_nodes_anim.at(i)->mNodeName.data == name)
            return ai_nodes_anim.at(i);
    }
    return nullptr;
}

int AnimatedModel::FindBoneIDByName(std::string name)
{
    for (int i = 0; i < bones.size(); i++)
    {
        if (bones.at(i).name == name)
            return i;
    }
    return -1;
}

void AnimatedModel::CreateBoneStructure()
{
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        for (int j = 0; j < scene->mMeshes[i]->mNumBones; j++)
        {
            std::string b_name = scene->mMeshes[i]->mBones[j]->mName.data;
            glm::mat4 b_mat = glm::transpose(AiToGLMMat4(scene->mMeshes[i]->mBones[j]->mOffsetMatrix));

            Bone bone(i, b_name, b_mat);

            bone.node = FindAiNode(b_name);
            bone.animNode = FindAiNodeAnim(b_name);

            if (bone.animNode == nullptr)
                std::cout << "No Animations were found for " + b_name << std::endl;

            //Finally, we push the Bone into our vector. Yay.
            bones.push_back(bone);
        }
    }

    for (int i = 0; i < bones.size(); i++)
    {
        std::string b_name = bones.at(i).name;
        std::string parent_name = FindAiNode(b_name)->mParent->mName.data;

        Bone* p_bone = FindBone(parent_name);

        bones.at(i).parent_bone = p_bone;

        if (p_bone == nullptr)
            std::cout << "Parent Bone for " << b_name << " does not exist (is nullptr)" << std::endl;

    }

    if (meshes->size() > 0)
        meshes->at(0).sceneLoaderSkeleton.Init(bones, globalInverseTransform);

    if (meshes->size() != 0)    //If there are, in fact, meshes.
        skeleton = meshes->at(0).sceneLoaderSkeleton;
}

void AnimatedModel::UpdateSkeleton()
{
    skeleton.Update();
}

void AnimatedModel::SetDeltaTime(float* delta)
{
    deltaTime = delta;
    skeleton.SetDeltaTime(delta);
}

void AnimatedModel::AddAnimation(Animation* anim)
{
    animation = anim;
    skeleton.PlayAnimation(*animation, true, true);
}

void AnimatedModel::AddAnimation(Animation& in_anim)
{
    animations.push_back(in_anim);
}

Animation* AnimatedModel::FindAnimation(std::string anim_to_find)
{
    for (int i = 0; i < animations.size(); i++)
    {
        if (animations.at(i).name == anim_to_find)
            return &animations.at(i);
    }
    return nullptr;
}

void AnimatedModel::PlayAnimation(Animation& anim, bool loop, bool reset_to_start)
{
    skeleton.PlayAnimation(anim, loop, reset_to_start);
}

void AnimatedModel::StopAnimating()
{
    skeleton.StopAnimating();
}


AnimatedModel::AnimatedModel()
{
}

AnimatedModel::AnimatedModel(std::string path)
{
    LoadModel(path);
}

AnimatedModel::AnimatedModel(std::vector<AnimatedMesh>* meshes, Joint root, int jointCount)
{
    this->meshes = meshes;
    this->rootJoint = root;
    this->joinCount = joinCount;
    animator = new Animator();
    this->rootJoint.CalculateInverseTransform(glm::mat4());
}

void AnimatedModel::Update()
{
    UpdateSkeleton();
    //animator->Update(rootJoint);
}

void AnimatedModel::DoAnimation(Animation* anim)
{
    animator->DoAnimation(*anim);
}

glm::mat4* AnimatedModel::GetJoinTransfoms()
{
    glm::mat4* joinMatrices = new glm::mat4[joinCount];
    AddJointToArray(rootJoint, joinMatrices);
    return joinMatrices;
}

void AnimatedModel::AddJointToArray(Joint headJoint, glm::mat4* matrices)
{
    matrices[headJoint.ID] = headJoint.GetJointTransform();
    for(std::list<Joint>::iterator it = headJoint.children.begin(); it != headJoint.children.end(); it++)
        AddJointToArray(*it, matrices);
}

void AnimatedModel::Render(std::vector<Light*> lights)
{
    int numDir, numPoint, numSpot;
    numDir = numPoint = numSpot = 0;

    matHandle.shader->use();

    for (unsigned int i = 0; i < meshes->size(); i++)
    {
        if (matHandle.type != MaterialType::INSTANCE)
        {
            int idLight;
            for (idLight = 0; idLight < lights.size(); idLight++)
            {
                if (lights.at(idLight)->GetType() == TypeLight::DIRL)
                {
                    std::string numLight = std::to_string(numDir);
                    meshes->at(i).material->ptrShader->setMat4("DirlightSpaceMatrix[" + numLight + "]", lights.at(idLight)->lightSpaceMatrix);
                    numDir++;
                }
                else if (lights.at(idLight)->GetType() == TypeLight::POINTLIGHT)
                {
                    std::string numLight = std::to_string(numPoint);
                    meshes->at(i).material->ptrShader->setMat4("PointlightSpaceMatrix[" + numLight + "]", lights.at(idLight)->lightSpaceMatrix);
                    numPoint++;
                }
                else if (lights.at(idLight)->GetType() == TypeLight::SPOTL)
                {
                    std::string numLight = std::to_string(numSpot);
                    meshes->at(i).material->ptrShader->setMat4("SpotlightSpaceMatrix[" + numLight + "]", lights.at(idLight)->lightSpaceMatrix);
                    numSpot++;
                }
            }

            //meshes->at(i).material->ptrShader->setMat4("model", transform->finalModelMatrix);
            meshes->at(i).material->ptrShader->setMat4("model", glm::mat4(1.0));
        }
        meshes->at(i).material->ptrShader->setMat4("gBones", skeleton.boneMats[0], skeleton.boneMats.size());
        meshes->at(i).Draw();
    }
}

void AnimatedModel::AddLights(std::vector<Light*> lights)
{
    matHandle.AddLights(lights);
}

void AnimatedModel::AddCamera(Camera* cam)
{
    matHandle.AddCamera(cam);
}
