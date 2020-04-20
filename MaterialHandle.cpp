#include "MaterialHandle.h"

MaterialHandle::MaterialHandle()
{
    this->type = MaterialType::LIT;
    this->shader = new Shader("shaders/nanosuit.vert", "shaders/nanosuit.frag");
}

MaterialHandle::MaterialHandle(Shader* sh)
{
    this->type = MaterialType::LIT;
    this->shader = sh;
}

void MaterialHandle::AddMaterialToList(Material* mat)
{
    listMaterials.push_back(mat);
}

void MaterialHandle::EditMaterial(MaterialComponent component, Shader& sh)
{
    switch (component)
    {
    case MaterialComponent::SHADER1:
        shader = &sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShader = shader;
        }
        break;
    case MaterialComponent::SHADER2:
        shader2 = &sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShader2 = shader2;
        }
        break;
    default:
        printf("ERROR::CHANGE_SHADER::RUN_FAILED\n");
        break;
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, float value)
{
    switch (component)
    {
    case MaterialComponent::SHININESS:
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->shininess = value;
        }
        break;
    case MaterialComponent::REFRACTIVE_INDEX:
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->refractiveIndex = value;
        }
        break;
    default:
        printf("ERROR::CHANGE_VALUE::RUN_FAILED\n");
        break;
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, glm::vec4 value)
{
    switch (component)
    {
    case MaterialComponent::OUTLINE_COLOR:
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->colorOutline = value;
        }
        break;
    default:
        printf("ERROR::CHANGE_VECTOR4::RUN_FAILED\n");
        break;
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, MaterialType type)
{
    this->type = type;

    if (component == MaterialComponent::TYPE)
    {
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->type = type;
        }
    }
    else
    {
        printf("ERROR::CHANGE_TYPE::RUN_FAILED\n");
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, std::vector<Texture> textures, unsigned int id)
{
    if (id == NULL)
    {
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->textures = textures;
        }
    }
    else
    {
        if (textures.size() > id)
        {
            listMaterials.at(id)->textures = textures;
        }
        else
        {
            printf("ERROR::CHANGE_TEXTURE::RUN_FAILED\n");
        }
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, bool value)
{
    this->isAmbientReflective = value;
    switch (component)
    {
        case MaterialComponent::A_REFLECTIVE:
            for (int i = 0; i < listMaterials.size(); i++)
                listMaterials.at(i)->isAmbientReflective = value;
            break;
        case MaterialComponent::A_REFRACTIVE:
            for (int i = 0; i < listMaterials.size(); i++)
                listMaterials.at(i)->isAmbientRefractive = value;
            break;
    }
}
