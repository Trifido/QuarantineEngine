#include "MaterialHandle.h"

MaterialHandle::MaterialHandle()
{
    this->numInstances = 0;
    this->isChangeNumInstances = false;
    this->type = MaterialType::LIT;
    this->deferred = new Shader("shaders/geometryPass.vert", "shaders/geometryPass.frag");
    this->forwardQA = new Shader("shaders/pbrShader1.vert", "shaders/pbrShader1.frag");
    this->forwardVolumeShadowShader = new Shader("shaders/pbrVolumeShadow.vert", "shaders/pbrVolumeShadow.frag");
    this->forward = new Shader("shaders/standardLighting.vert", "shaders/standardLighting.frag");
    this->shader2 = new Shader("shaders/outline.vert", "shaders/outline.frag");
    this->shaderShadow = new Shader("shaders/shadow.vert", "shaders/shadow.frag");
    this->shaderPointShadow = new Shader("shaders/pointShadow.vert", "shaders/pointShadow.gm", "shaders/pointShadow.frag");
    this->shaderVolumeShadow = new Shader("shaders/volumeGeometryShadow.vert", "shaders/volumeGeometryShadow.gm", "shaders/volumeGeometryShadow.frag");
    this->occlussionShader = new Shader("shaders/occlusionShader.vert", "shaders/occlusionShader.frag");
    this->shader = forwardQA;
    this->depthMapShader = new Shader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
}

MaterialHandle::MaterialHandle(Shader* sh)
{
    this->numInstances = 0;
    this->isChangeNumInstances = false;
    this->type = MaterialType::LIT;
    this->deferred = new Shader("shaders/geometryPass.vert", "shaders/geometryPass.frag");
    this->forward = sh;
    this->shader = forward;
    this->shader2 = new Shader("shaders/outline.vert", "shaders/outline.frag");
    this->shaderShadow = new Shader("shaders/shadow.vert", "shaders/shadow.frag");
    this->shaderPointShadow = new Shader("shaders/pointShadow.vert", "shaders/pointShadow.gm", "shaders/pointShadow.frag");
    this->shaderVolumeShadow = new Shader("shaders/volumeGeometryShadow.vert", "shaders/volumeGeometryShadow.gm", "shaders/volumeGeometryShadow.frag");
    this->occlussionShader = new Shader("shaders/occlusionShader.vert", "shaders/occlusionShader.frag");
    this->forwardVolumeShadowShader = new Shader("shaders/pbrVolumeShadow.vert", "shaders/pbrVolumeShadow.frag");
    this->forwardQA = new Shader("shaders/pbrShader1.vert", "shaders/pbrShader1.frag");
    this->depthMapShader = new Shader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
}

void MaterialHandle::AddMaterialToList(Material* mat)
{
    listMaterials.push_back(mat);
}

void MaterialHandle::AddLights(std::vector<Light*> lights)
{
    this->deferred->AddLight(lights);
    this->forwardQA->AddLight(lights);
    this->forward->AddLight(lights);
    this->shader->AddLight(lights);
    this->forwardVolumeShadowShader->AddLight(lights);
    this->shader2->AddLight(lights);
    this->occlussionShader->AddLight(lights);
    this->depthMapShader->AddLight(lights);
}

void MaterialHandle::AddCamera(Camera* mainCamera)
{
    this->deferred->AddCamera(mainCamera);
    this->forwardQA->AddCamera(mainCamera);
    this->forward->AddCamera(mainCamera);
    this->shader->AddCamera(mainCamera);
    this->shader2->AddCamera(mainCamera);
    this->occlussionShader->AddCamera(mainCamera);
    this->forwardVolumeShadowShader->AddCamera(mainCamera);
    this->depthMapShader->AddCamera(mainCamera);
}

void MaterialHandle::EditMaterial(MaterialComponent component, Shader* sh)
{
    switch (component)
    {
    case MaterialComponent::SHADER1:
        shader = sh;

        if (current_render_mode == RenderType::FORWARD_RENDER)
            forward = shader;
        else
            deferred = shader;

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
    case MaterialComponent::SHADER_FORWARD:
        forward = sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShader = forward;
        }
        break;
    case MaterialComponent::SHADER_DEFERRED:
        deferred = sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShader = deferred;
        }
        break;
    case MaterialComponent::SHADER_POINT_SHADOW:
        shaderPointShadow = sh;
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShaderPointShadow = shaderPointShadow;
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
    case MaterialComponent::P_DISPLACEMENT:
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->parallax_displacement = value;
        }
        break;
    case MaterialComponent::MIN_UV:
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->min_uv = value;
        }
        break;
    case MaterialComponent::MAX_UV:
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->max_uv = value;
        }
        break;
    case MaterialComponent::BLOOM_BRIGHTNESS:
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->bloomBrightness = value;
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

void MaterialHandle::EditMaterial(MaterialComponent component, DrawMode type, bool guimode)
{
    if (component == MaterialComponent::DRAW_MODE)
    {
        for (int i = 0; i < listMaterials.size(); i++)
        {
            if(generalDrawMode || guimode)
                listMaterials.at(i)->drawtype = type;
        }
    }
    else
    {
        printf("ERROR::CHANGE_DRAW_MODE::RUN_FAILED\n");
    }
}

void MaterialHandle::EditMaterial(MaterialComponent component, RenderType renderTypeSelected)
{
    if(renderTypeSelected != current_render_mode)
    {
        if (renderTypeSelected == RenderType::FORWARD_RENDER)
        {
            this->shader = forward;
        }
        else if (renderTypeSelected == RenderType::FORWARD_QUALITY_RENDER)
        {
            this->shader = forwardQA;
        }
        else
        {
            this->shader = deferred;
        }

        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->ptrShader = shader;
        }

        current_render_mode = renderTypeSelected;
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

void MaterialHandle::EditMaterial(MaterialComponent component, std::vector<ProceduralTexture> procedural_textures, unsigned int id)
{
    if (id == NULL)
    {
        for (int i = 0; i < listMaterials.size(); i++)
        {
            listMaterials.at(i)->procedural_textures = procedural_textures;
        }
    }
    else
    {
        if (procedural_textures.size() > id)
        {
            listMaterials.at(id)->procedural_textures = procedural_textures;
        }
        else
        {
            printf("ERROR::CHANGE_PROCEDURAL_TEXTURE::RUN_FAILED\n");
        }
    }
}

void MaterialHandle::ActivateShadowMap(unsigned int idTexShadow, unsigned int idLight, TypeLight type)
{
    shader->use();
    for (int i = 0; i < listMaterials.size(); i++)
    {
        listMaterials.at(i)->ActivateShadowTexture(idTexShadow, idLight, type);
    }
}

void MaterialHandle::ActivateIrradianceMap(unsigned int idTexIrradiance, unsigned int idTexPrefilter, unsigned int idTexBrdf)
{
    shader->use();
    for (int i = 0; i < listMaterials.size(); i++)
    {
        listMaterials.at(i)->ActivateIrradianceMap(idTexIrradiance, idTexPrefilter, idTexBrdf);
    }
}

void MaterialHandle::ChangeCurrentShader(MaterialComponent component)
{
    switch (component)
    {
    case MaterialComponent::SHADER_DEFERRED:
        shader = deferred;
        break;
    case MaterialComponent::SHADER_FORWARD:
        shader = forward;
        break;
    case MaterialComponent::SHADER_FORWARD_QA:
        shader = forwardQA;
        break;
    case MaterialComponent::SHADER_VOLUME_SHADOW:
        shader = forwardVolumeShadowShader;
        break;
    default:
        shader = forwardQA;
        break;
    }

    for (int i = 0; i < listMaterials.size(); i++)
    {
        listMaterials.at(i)->ptrShader = shader;
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
