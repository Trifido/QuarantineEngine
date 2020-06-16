#include "BoundingLight.h"

BoundingLight::BoundingLight()
{ 
    //matHandle.type = MaterialType::BOUNDING_VOLUME;
    matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::BOUNDING_VOLUME);
    matHandle.EditMaterial(MaterialComponent::SHADER_DEFERRED, new Shader("shaders/lightVolumePass.vert", "shaders/lightVolumePass.frag"));
    loadModel("./resources/3DModels/sphere/sphere.obj");
    mesh->SetBoundingLight(true);  
    light = new Light(TypeLight::POINTLIGHT, glm::vec3(-1.0f, 0.0f, 2.0f));
    light->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(1.0f));
    light->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(1.0f));

    //ScaleTo(glm::vec3(light->GetRadius() / 2.0f));
}

void BoundingLight::Draw(bool isOutline)
{
    //matHandle.shader->use();
    mesh->material->ptrShader->use();
    mesh->material->ptrShader->setMat4("model", model);
    mesh->Draw();
}

void BoundingLight::loadModel(std::string path)
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

void BoundingLight::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* amesh = scene->mMeshes[node->mMeshes[i]];
        mesh = processMesh(amesh, scene);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh* BoundingLight::processMesh(aiMesh* mesh, const aiScene* scene)
{
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
     
    return new Mesh(vertices, indices, matHandle);
}

void BoundingLight::Rotation(float radians, glm::vec3 axis)
{
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(radians), axis);
}

void BoundingLight::TranslationTo(glm::vec3 position)
{
    this->model = glm::translate(this->model, position);
    light->EditLightComponent(LightComponent::LIGHT_POSITION, position);
}

void BoundingLight::ScaleTo(glm::vec3 scale)
{
    this->model = glm::scale(this->model, scale);
}

void BoundingLight::ResetModel()
{
    this->model = glm::mat4(1.0f);
}

void BoundingLight::DeleteGPUInfo()
{ 
    mesh->DelteGPUInfo();
}  

void BoundingLight::AddMaterial(Material* mat)
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

void BoundingLight::AddMaterial(Material* mat, std::vector<Texture> textures)
{
    matHandle.EditMaterial(MaterialComponent::TYPE, mat->type);
    matHandle.EditMaterial(MaterialComponent::SHADER1, mat->ptrShader);
    matHandle.EditMaterial(MaterialComponent::SHADER2, mat->ptrShader2);
    matHandle.EditMaterial(MaterialComponent::SHININESS, mat->shininess);
    matHandle.EditMaterial(MaterialComponent::OUTLINE_COLOR, mat->colorOutline);
    matHandle.EditMaterial(MaterialComponent::TEXTURE, textures);

}

void BoundingLight::AddCamera(Camera* cam)
{
    matHandle.shader->AddCamera(cam);
    if (matHandle.type == MaterialType::OUTLINE || matHandle.type == MaterialType::NORMALS)
    {
        matHandle.shader2->AddCamera(cam); 
    }
}
