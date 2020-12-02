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
    bloomBrightness = 1.0f;
    refractiveIndex = 1.0f;
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
    min_uv = 0.0f;
    max_uv = 1.0f;
    bloomBrightness = 1.0f;
    refractiveIndex = 1.0f;
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
    min_uv = 0.0f;
    max_uv = 1.0f;
    bloomBrightness = 1.0f;
    refractiveIndex = 1.0f;
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
    min_uv = 0.0f;
    max_uv = 1.0f;
    bloomBrightness = 1.0f;
    refractiveIndex = 1.0f;
}

Material::Material(Shader* shader, std::vector<ProceduralTexture> textures)
{
    type = MaterialType::LIT;
    drawtype = DrawMode::DTRIANGLES;
    ptrShader = shader;
    this->procedural_textures = textures;
    shininess = 1.0f;
    colorOutline = glm::vec4(1.0f);
    numInstances = 0;
    parallax_displacement = 0.0f;
    min_uv = 0.0f;
    max_uv = 1.0f;
    bloomBrightness = 1.0f;
    refractiveIndex = 1.0f;
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
    min_uv = 0.0f;
    max_uv = 1.0f;
    bloomBrightness = 1.0f;
    refractiveIndex = 1.0f;
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
    min_uv = mat.min_uv;
    max_uv = mat.max_uv;
    bloomBrightness = mat.bloomBrightness;
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
        //if(idLight == 0)
        //    ptrShader->setInt("shadowCubeMap", numTextures);
        //else
        //    ptrShader->setInt("shadowCubeMap2", numTextures);
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

void Material::ActivateIrradianceMap(unsigned int idTexIrradiance, unsigned int idTexPrefilter, unsigned int idTexBrdf)
{
    ptrShader->use();
    ptrShader->setInt("irradianceMap", numTextures);
    glActiveTexture(GL_TEXTURE0 + numTextures);
    glBindTexture(GL_TEXTURE_CUBE_MAP, idTexIrradiance);
    numTextures++;
    ptrShader->setInt("prefilterMap", numTextures);
    glActiveTexture(GL_TEXTURE0 + numTextures);
    glBindTexture(GL_TEXTURE_CUBE_MAP, idTexPrefilter);
    numTextures++;
    ptrShader->setInt("brdfLUT", numTextures);
    glActiveTexture(GL_TEXTURE0 + numTextures);
    glBindTexture(GL_TEXTURE_2D, idTexBrdf);
    numTextures++;
}

void Material::AddMultTextures(std::vector<Texture> texturesIN)
{  
    for (int i = 0; i < texturesIN.size(); i++)
    { 
        this->textures.push_back(texturesIN[i]);
    } 
}

void Material::AddMultProceduralTextures(std::vector<ProceduralTexture> texturesIN)
{  
    for (int i = 0; i < texturesIN.size(); i++)
    { 
        this->procedural_textures.push_back(texturesIN[i]);
    } 
}

void Material::AssignRenderTextures()
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    unsigned int emissiveNr = 0;
    unsigned int aoNr = 0;
    unsigned int metallicNr = 0;
    unsigned int roughnessNr = 0;
    unsigned int bumpNr = 0;
    unsigned int noiseNr = 0;
    unsigned int worleyNr = 0;
    unsigned int perlinNr = 0;

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
        case TypeTexture::AO:
            name = "material.ao[" + std::to_string(aoNr++) + "]";
            break;
        case TypeTexture::METALLIC:
            name = "material.metallic[" + std::to_string(metallicNr++) + "]";
            break;
        case TypeTexture::ROUGHNESS:
            name = "material.roughness[" + std::to_string(roughnessNr++) + "]";
            break;
        case TypeTexture::BUMP:
            name = "material.bump[" + std::to_string(bumpNr++) + "]";
            break;
        case TypeTexture::NOISE:
            name = "material.noise[" + std::to_string(noiseNr++) + "]";
            break;
        }

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);

        ptrShader->use();
        ptrShader->setInt((name).c_str(), i);
    }

    for (unsigned int i = 0; i < procedural_textures.size(); i++)
    {
        std::string name;

        switch (procedural_textures[i].typeNoise)
        {
        default:
        case NoiseType::PERLIN:
            name = "material.perlin[" + std::to_string(perlinNr++) + "]";
            break;
        case NoiseType::WORLEY:
            name = "material.worley[" + std::to_string(worleyNr++) + "]";
            break;
        }
    }

    ptrShader->use();
    ptrShader->setInt("material.num_diffuse", diffuseNr);
    ptrShader->setInt("material.num_specular", specularNr);
    ptrShader->setInt("material.num_normal", normalNr);
    ptrShader->setInt("num_normal", normalNr);
    ptrShader->setInt("material.num_height", heightNr);
    ptrShader->setInt("material.num_emissive", emissiveNr);
    ptrShader->setInt("material.num_ao", aoNr);
    ptrShader->setInt("material.num_metallic", metallicNr);
    ptrShader->setInt("material.num_roughness", roughnessNr);
    ptrShader->setInt("material.num_bump", bumpNr);
    ptrShader->setInt("material.num_noise", noiseNr);
    ptrShader->setInt("material.num_worley", worleyNr);
    ptrShader->setInt("material.num_perlin", perlinNr);
    ptrShader->setFloat("material.shininess", shininess);
    ptrShader->setBool("material.blinn", isBlinnShading);
    ptrShader->setBool("material.isAmbientReflective", isAmbientReflective);
    ptrShader->setBool("material.isAmbientRefractive", isAmbientRefractive);
    ptrShader->setInt("material.isBoundingLight", isBounding);
    ptrShader->setFloat("material.refractiveIndex", refractiveIndex);
    ptrShader->setFloat("material.heightScale", parallax_displacement);
    ptrShader->setFloat("generalAmbient", 0.4f);
    ptrShader->setFloat("material.min_uv", min_uv);
    ptrShader->setFloat("material.max_uv", max_uv);
    ptrShader->setFloat("material.bloomBrightness", bloomBrightness);

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

void Material::AssignRenderTextures(Shader* sh)
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    unsigned int emissiveNr = 0;
    unsigned int aoNr = 0;
    unsigned int metallicNr = 0;
    unsigned int roughnessNr = 0;
    unsigned int bumpNr = 0;
    unsigned int noiseNr = 0;
    unsigned int worleyNr = 0;
    unsigned int perlinNr = 0;

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
        case TypeTexture::AO:
            name = "material.ao[" + std::to_string(aoNr++) + "]";
            break;
        case TypeTexture::METALLIC:
            name = "material.metallic[" + std::to_string(metallicNr++) + "]";
            break;
        case TypeTexture::ROUGHNESS:
            name = "material.roughness[" + std::to_string(roughnessNr++) + "]";
            break;
        case TypeTexture::BUMP:
            name = "material.bump[" + std::to_string(bumpNr++) + "]";
            break;
        case TypeTexture::NOISE:
            name = "material.noise[" + std::to_string(noiseNr++) + "]";
            break;
        }

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);

        sh->use();
        sh->setInt((name).c_str(), i);
    }

    for (unsigned int i = 0; i < procedural_textures.size(); i++)
    {
        std::string name;

        switch (procedural_textures[i].typeNoise)
        {
        default:
        case NoiseType::PERLIN:
            name = "material.perlin[" + std::to_string(perlinNr++) + "]";
            break;
        case NoiseType::WORLEY:
            name = "material.worley[" + std::to_string(worleyNr++) + "]";
            break;
        }
    }

    sh->use();
    sh->setInt("material.num_diffuse", diffuseNr);
    sh->setInt("material.num_specular", specularNr);
    sh->setInt("material.num_normal", normalNr);
    sh->setInt("num_normal", normalNr);
    sh->setInt("material.num_height", heightNr);
    sh->setInt("material.num_emissive", emissiveNr);
    sh->setInt("material.num_ao", aoNr);
    sh->setInt("material.num_metallic", metallicNr);
    sh->setInt("material.num_roughness", roughnessNr);
    sh->setInt("material.num_bump", bumpNr);
    sh->setInt("material.num_noise", noiseNr);
    sh->setInt("material.num_worley", worleyNr);
    sh->setInt("material.num_perlin", perlinNr);
    sh->setFloat("material.shininess", shininess);
    sh->setBool("material.blinn", isBlinnShading);
    sh->setBool("material.isAmbientReflective", isAmbientReflective);
    sh->setBool("material.isAmbientRefractive", isAmbientRefractive);
    sh->setInt("material.isBoundingLight", isBounding);
    sh->setFloat("material.refractiveIndex", refractiveIndex);
    sh->setFloat("material.heightScale", parallax_displacement);
    sh->setFloat("generalAmbient", 0.4f);
    sh->setFloat("material.min_uv", min_uv);
    sh->setFloat("material.max_uv", max_uv);
    sh->setFloat("material.bloomBrightness", bloomBrightness);

    if (isAmbientReflective || isAmbientRefractive)
    {
        glActiveTexture(GL_TEXTURE0 + textures.size());
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->ID);

        sh->use();
        sh->setInt("skybox", textures.size());
    }

    sh->use();

    sh->ActivateCamera();
    sh->ActivateLights();
}
