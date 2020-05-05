#include "MaterialHandle.h"

MaterialHandle::MaterialHandle()
{
    this->numInstances = 0;
    this->isChangeNumInstances = false;
    this->type = MaterialType::LIT;
    this->shader = new Shader("shaders/standardShadow.vert", "shaders/standardShadow.frag");
    this->shaderShadow = new Shader("shaders/shadow.vert", "shaders/shadow.frag");
}

MaterialHandle::MaterialHandle(Shader* sh)
{
    this->numInstances = 0;
    this->isChangeNumInstances = false;
    this->type = MaterialType::LIT;
    this->shader = sh;
    this->shaderShadow = new Shader("shaders/shadow.vert", "shaders/shadow.frag");
}

void MaterialHandle::AddMaterialToList(Material* mat)
{
    listMaterials.push_back(mat);
}

void MaterialHandle::EditMaterial(MaterialComponent component, Shader* sh)
{
    switch (component)
    {
    case MaterialComponent::SHADER1:
        shader = sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShader = shader;
        }
        break;
    case MaterialComponent::SHADER2:
        shader2 = sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShader2 = shader2;
        }
        break;
    case MaterialComponent::SHADER_SHADOW:
        shaderShadow = sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShaderShadow = shaderShadow;
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
    case MaterialComponent::NUM_INSTANCES:
        if (numInstances != (int)value && (int)value > 0)
        {
            numInstances = value;
            isChangeNumInstances = true;

            //Cambiamos el numero de instancias
            for (int i = 0; i < listMaterials.size(); i++)
            {
                listMaterials.at(i)->numInstances = this->numInstances;
            }
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

void MaterialHandle::EditMaterial(MaterialComponent component, MaterialType type, int numInstances)
{
    this->type = type;

    if (component == MaterialComponent::TYPE)
    {
        if (type == MaterialType::INSTANCE)
            this->numInstances = numInstances;

        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->type = type;
            listMaterials.at(i)->numInstances = this->numInstances;
        }
    }
    else
    {
        printf("ERROR::CHANGE_TYPE::RUN_FAILED\n");
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, DrawMode type)
{
    if (component == MaterialComponent::DRAW_MODE)
    {
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->drawtype = type;
        }
    }
    else
    {
        printf("ERROR::CHANGE_DRAW_MODE::RUN_FAILED\n");
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

void MaterialHandle::ActivateShadowMap(unsigned int idTexShadow)
{
    shader->use();
    for (int i = 0; i < listMaterials.size(); i++)
    {
        listMaterials.at(i)->ActivateShadowTexture(idTexShadow);
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, bool value)
{
    switch (component)
    {
        case MaterialComponent::A_REFLECTIVE:
            this->isAmbientReflective = value;
            for (int i = 0; i < listMaterials.size(); i++)
                listMaterials.at(i)->isAmbientReflective = value;
            break;
        case MaterialComponent::A_REFRACTIVE:
            for (int i = 0; i < listMaterials.size(); i++)
                listMaterials.at(i)->isAmbientRefractive = value;
            break;
        case MaterialComponent::BLINN:
            for (int i = 0; i < listMaterials.size(); i++)
                listMaterials.at(i)->isBlinnShading = value;
            break;
    }
}
