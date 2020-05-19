#include "Light.h"

Light::Light()
{
    type = TypeLight::POINTLIGHT;
    this->Init();
    this->ComputeShadowProjection();
}

Light::Light(TypeLight newType)
{
    this->Init();
    this->SetType(newType);
    this->ComputeShadowProjection();
}

Light::Light(TypeLight newType, glm::vec3 newPos)
{
    this->Init();
    this->type = newType;
    this->position = newPos;
    this->ComputeShadowProjection();
}

void Light::EditLightComponent(LightComponent lc, glm::vec3 value)
{
    switch (lc)
    {
    case LIGHT_POSITION:
        position = value;
        if (type == TypeLight::DIRL)
        {
            lightView = glm::lookAt(position, glm::normalize(direction) + position, glm::vec3(0.0f, 1.0f, 0.0f));
            lightSpaceMatrix = lightProjection * lightView;
        }
        else
        {
            ComputeShadowProjection();
        }
        break;
    case LIGHT_DIRECTION:
        direction = value; 
        lightView = glm::lookAt(position, glm::normalize(direction) + position, glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
        ComputeShadowProjection();
        break;
    case LIGHT_DIFFUSE:
        diffuse = value;
        break;
    case LIGHT_SPECULAR:
        specular = value;
        break;
    case LIGHT_AMBIENT:
        ambient = value;
        break;
    default:
        break;
    }
}

void Light::EditLightComponent(LightComponent lc, glm::vec4 value)
{
    switch (lc)
    {
    case LIGHT_PROJSHADOW:
        projDimension = value;
        if (type == TypeLight::DIRL)
            lightProjection = glm::ortho(projDimension.x, projDimension.y, projDimension.z, projDimension.w, near_plane, far_plane);
        lightSpaceMatrix = lightProjection * lightView;
        break;
    default:
        break;
    }
}

void Light::EditLightComponent(LightComponent lc, float value)
{
    switch (lc)
    {
    case LIGHT_NEAR_EFFECT:
        near_plane = value;
        if (type == TypeLight::DIRL)
            lightProjection = glm::ortho(projDimension.x, projDimension.y, projDimension.z, projDimension.w, near_plane, far_plane);
        lightSpaceMatrix = lightProjection * lightView;
        break;
    case LIGHT_FAR_EFFECT:
        far_plane = value;
        if (type == TypeLight::DIRL)
            lightProjection = glm::ortho(projDimension.x, projDimension.y, projDimension.z, projDimension.w, near_plane, far_plane);
        lightSpaceMatrix = lightProjection * lightView;
        break;
    case LIGHT_LINEAR:
        linear = value;
        break;
    case LIGHT_QUADRATIC:
        quadratic = value;
        break;
    case LIGHT_ASPECT:
        aspect = value;
        ComputeShadowCubeMapProjection();
        lightSpaceMatrix = lightProjection * lightView;
        break;
    default:
        break;
    }
}

void Light::EditLightComponent(LightComponent lc, bool value)
{
    if(lc == LightComponent::LIGHT_CAST_SHADOW)
        isCastShadow = value;
}

void Light::ComputeShadowProjection()
{
    switch (type)
    {
    case POINTLIGHT:
        lightProjection = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
        ComputeShadowCubeMapProjection();
        break;
    case DIRL:
        lightProjection = glm::ortho(projDimension.x, projDimension.y, projDimension.z, projDimension.w, near_plane, far_plane);
        glm::vec3 pointView = glm::normalize(direction) + this->position;
        lightView = glm::lookAt(this->position, pointView, glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
        break;
    case SPOTL:
        lightProjection = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
        glm::vec3 pointView2 = glm::normalize(direction) + this->position;
        lightView = glm::lookAt(this->position, pointView2, glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
        break;
    case SPOTFPSL:
        break;
    default:
        break;
    }
}

void Light::ComputeShadowCubeMapProjection()
{
    if (!lightSpaceMatrices.empty())
        lightSpaceMatrices.clear();
    lightSpaceMatrices.push_back(lightProjection * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    lightSpaceMatrices.push_back(lightProjection * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    lightSpaceMatrices.push_back(lightProjection * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    lightSpaceMatrices.push_back(lightProjection * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    lightSpaceMatrices.push_back(lightProjection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    lightSpaceMatrices.push_back(lightProjection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
}

void Light::Init()
{
    //Coords & Dir 
    direction = glm::vec3(-0.5f, -1.0f, 0.0f); //glm::vec3(-0.2f, -1.0f, -0.3f);
    //Color Light
    ambient = glm::vec3(0.05f);
    diffuse = glm::vec3(0.8f);
    specular = glm::vec3(1.0f);
    //Attenuation & Intensity
    constant = 1.0f;
    linear = 0.09f;
    quadratic = 0.032f;
    //Spot
    cutOff = glm::cos(glm::radians(12.5f));
    outerCutOff = glm::cos(glm::radians(17.5f));

    near_plane = 0.1f;
    far_plane = 100.0f;
    aspect = 1.0f;
    projDimension = glm::vec4(-10.0f, 10.0f, -10.0f, 10.0f);
}
