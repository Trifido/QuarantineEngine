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
    position = glm::vec3(0.0f);
    //direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    direction = glm::vec3(0.2f, 1.0f, 0.3f);
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
}

Light::Light(TypeLight newType, glm::vec3 newPos)
{
    //Coords & Dir
    //position = glm::vec3(0.0f);
    //direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    direction = glm::vec3(0.2f, 1.0f, 0.3f);
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
    this->SetPosition(newPos);
}
