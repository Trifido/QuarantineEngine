#include "Light.h"



Light::Light()
{
    type = TypeLight::POINTLIGHT;
    //Coords & Dir
    position = glm::vec3(0.0f);
    direction = glm::vec3(-0.2f, -1.0f, -0.3f);
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
}

Light::Light(TypeLight newType)
{
    //Coords & Dir
    position = glm::vec3(0.0f); //glm::vec3(-2.0f, 4.0f, -1.0f);
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
    this->SetType(newType);

    near_plane = 1.0f;
    far_plane = 7.5f;
    projDimension = glm::vec4(-10.0f, 10.0f, -10.0f, 10.0f);
    if(type == TypeLight::DIRL)
        lightProjection = glm::ortho(projDimension.x, projDimension.y, projDimension.z, projDimension.w, near_plane, far_plane);

    glm::vec3 pointView = glm::normalize(direction) + position;
    lightView = glm::lookAt(position, pointView, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
}

Light::Light(TypeLight newType, glm::vec3 newPos)
{
    //Coords & Dir 
    position = newPos;
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
    this->SetType(newType);

    near_plane = 1.0f;
    far_plane = 7.5f;
    projDimension = glm::vec4(-10.0f, 10.0f, -10.0f, 10.0f);
    if (type == TypeLight::DIRL)
        lightProjection = glm::ortho(projDimension.x, projDimension.y, projDimension.z, projDimension.w, near_plane, far_plane);

    glm::vec3 pointView = glm::normalize(direction) + position;
    lightView = glm::lookAt(position, pointView, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
}

void Light::EditLightComponent(LightComponent lc, glm::vec3 value)
{
    switch (lc)
    {
    case LIGHT_POSITION:
        position = value; 
        lightView = glm::lookAt(position, glm::normalize(direction) + position, glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
        break;
    case LIGHT_DIRECTION:
        direction = value; 
        lightView = glm::lookAt(position, glm::normalize(direction) + position, glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
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
    default:
        break;
    }
}
