#include "Material.h"

Material::Material()
{
    type = MaterialType::LIT;
    ptrShader = new Shader("shaders/nanosuit.vert", "shaders/nanosuit.frag");
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
}

Material::Material(Shader* shader)
{
    type = MaterialType::LIT;
    ptrShader = shader;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
}

Material::Material(Shader* shader, std::vector<Texture> textures)
{
    type = MaterialType::LIT;
    ptrShader = shader;
    this->textures = textures;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
}

Material::Material(Shader* shader, Shader* shader2, std::vector<Texture> textures)
{
    type = MaterialType::OUTLINE;
    ptrShader = shader;
    ptrShader2 = shader2;
    this->textures = textures;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
}

void Material::AddShader(Shader* sh)
{
    ptrShader = sh;
}

void Material::AddTexture(Texture texture)
{
    textures.push_back(texture);
}
void Material::AddMultTextures(std::vector<Texture> texturesIN)
{  
    for (int i = 0; i < texturesIN.size(); i++)
    { 
        this->textures.push_back(texturesIN[i]);
    } 
}

void Material::AssignRenderTextures()
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    unsigned int emissiveNr = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    { 

        std::string name;

        switch (textures[i].type)
        {
        default:
        case TypeTexture::DIFFUSE:
            name = "material.diffuse[" + std::to_string(diffuseNr++) + "]";
            break;
        case TypeTexture::SPECULAR:
            name = "material.specular[" + std::to_string(specularNr++) + "]";
            break;
        case TypeTexture::NORMAL:
            name = "material.normal[" + std::to_string(normalNr++) + "]";
            break;
        case TypeTexture::HEIGHT:
            name = "material.height[" + std::to_string(heightNr++) + "]";
            break;
        case TypeTexture::EMISSIVE:
            name = "material.emissive[" + std::to_string(emissiveNr++) + "]";
            break;
        }

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);

        ptrShader->use();
        ptrShader->setInt((name).c_str(), i);
    }

    ptrShader->setInt("material.num_diffuse", diffuseNr);
    ptrShader->setInt("material.num_specular", specularNr);
    ptrShader->setInt("material.num_normal", normalNr);
    ptrShader->setInt("num_normal", normalNr);
    ptrShader->setInt("material.num_height", heightNr);
    ptrShader->setInt("material.num_emissive", emissiveNr);
    ptrShader->setFloat("material.shininess", shininess);
    ptrShader->setBool("material.isAmbientReflective", isAmbientReflective);
    ptrShader->setBool("material.isAmbientRefractive", isAmbientRefractive);
    ptrShader->setFloat("material.refractiveIndex", refractiveIndex);

    if (isAmbientReflective || isAmbientRefractive)
    {
        glActiveTexture(GL_TEXTURE0 + textures.size());
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->ID);

        ptrShader->use();
        ptrShader->setInt("skybox", textures.size());
    }

    ptrShader->use();

    ptrShader->ActivateCamera();
    ptrShader->ActivateLights();

}
