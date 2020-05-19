#include "Material.h"

Material::Material()
{
    type = MaterialType::LIT;
    drawtype = DrawMode::DTRIANGLES;
    ptrShader = new Shader("shaders/standardLighting.vert", "shaders/standardLighting.frag");
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
    numInstances = 0;
    parallax_displacement = 0.0f;
}

Material::Material(Shader* shader)
{
    type = MaterialType::LIT;
    drawtype = DrawMode::DTRIANGLES;
    ptrShader = shader;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
    numInstances = 0;
    parallax_displacement = 0.0f;
}

Material::Material(Shader* shader, Shader* shader2, MaterialType mattype)
{
    type = mattype;
    drawtype = DrawMode::DTRIANGLES;
    ptrShader = shader;
    ptrShader2 = shader2;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
    numInstances = 0; 
    parallax_displacement = 0.0f;
}

Material::Material(Shader* shader, std::vector<Texture> textures)
{
    type = MaterialType::LIT;
    drawtype = DrawMode::DTRIANGLES;
    ptrShader = shader;
    this->textures = textures;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
    numInstances = 0;
    parallax_displacement = 0.0f;
}

Material::Material(Shader* shader, Shader* shader2, std::vector<Texture> textures)
{
    type = MaterialType::OUTLINE;
    drawtype = DrawMode::DTRIANGLES;
    ptrShader = shader;
    ptrShader2 = shader2;
    this->textures = textures;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
    numInstances = 0;
    parallax_displacement = 0.0f;
}

void Material::CopyMaterial(Material mat)
{
    type = mat.type;
    drawtype = mat.drawtype;
    ptrShader = mat.ptrShader;
    ptrShader2 = mat.ptrShader2;
    ptrShaderShadow = mat.ptrShaderShadow;
    ptrShaderPointShadow = mat.ptrShaderPointShadow;
    this->textures = mat.textures;
    shininess = mat.shininess;
    colorOutline = mat.colorOutline;
    numInstances = mat.numInstances;
    skyboxTexture = mat.skyboxTexture;
    refractiveIndex = mat.refractiveIndex;
    isAmbientReflective = mat.isAmbientReflective;
    isAmbientRefractive = mat.isAmbientRefractive;
    isBlinnShading = mat.isBlinnShading;
    parallax_displacement = mat.parallax_displacement;
}

void Material::AddShader(Shader* sh)
{
    ptrShader = sh;
}

void Material::AddTexture(Texture texture)
{
    textures.push_back(texture);
}

void Material::ActivateShadowTexture(unsigned int idTexShadow, int idLight, TypeLight type)
{
    ptrShader->use();

    switch (type)
    {
    case TypeLight::DIRL:
        ptrShader->setInt("dirLights[" + std::to_string(idLight) + "].shadowMap", numTextures);
        break;
    case TypeLight::POINTLIGHT:
        ptrShader->setInt("pointLights[" + std::to_string(idLight) + "].shadowCubeMap", numTextures);
        break;
    case TypeLight::SPOTL:
        ptrShader->setInt("spotLights[" + std::to_string(idLight) + "].shadowMap", numTextures);
        break;
    default:
        break;
    }

    glActiveTexture(GL_TEXTURE0 + numTextures);
    if (type == TypeLight::POINTLIGHT)
        glBindTexture(GL_TEXTURE_CUBE_MAP, idTexShadow);
    else
        glBindTexture(GL_TEXTURE_2D, idTexShadow);

    numTextures++;
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

    numTextures = textures.size();

    for (unsigned int i = 0; i < numTextures; i++)
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
    ptrShader->setBool("material.blinn", isBlinnShading);
    ptrShader->setBool("material.isAmbientReflective", isAmbientReflective);
    ptrShader->setBool("material.isAmbientRefractive", isAmbientRefractive);
    ptrShader->setFloat("material.refractiveIndex", refractiveIndex);
    ptrShader->setFloat("material.heightScale", parallax_displacement);

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
