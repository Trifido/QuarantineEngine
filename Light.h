#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "HeadersRequeriments.h"

enum LightComponent {
    LIGHT_NEAR_EFFECT,
    LIGHT_FAR_EFFECT,
    LIGHT_POSITION,
    LIGHT_DIRECTION,
    LIGHT_DIFFUSE,
    LIGHT_SPECULAR,
    LIGHT_AMBIENT,
    LIGHT_LINEAR,
    LIGHT_QUADRATIC,
    LIGHT_PROJSHADOW
};

enum TypeLight {
    POINTLIGHT = 0,
    DIRL = 1,
    SPOTL = 2,
    SPOTFPSL = 3
};

class Light
{
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    TypeLight type;

    float near_plane;
    float far_plane;
    glm::vec4 projDimension;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    
public:
    glm::mat4 lightSpaceMatrix;
    Light();
    Light(TypeLight newType);
    Light(TypeLight newType, glm::vec3 newPos);
    void inline SetPosition(glm::vec3& newPos) { position = newPos; }
    void inline SetDirection(glm::vec3& newDir) { direction = newDir; }
    void inline SetDiffuse(glm::vec3& newDiff) { diffuse = newDiff; }
    void inline SetSpecular(glm::vec3& newSpec) { specular = newSpec; }
    void inline SetAmbient(glm::vec3& newAmb) { ambient = newAmb; }
    void inline SetType(TypeLight newType) { type = newType; }
    void inline SetCutOff(float newCut) { cutOff = newCut; }
    void inline SetOuterCutOff(float newout) { outerCutOff = newout; }
    void inline SetLinear(float newLinear) { linear = newLinear; }
    void inline SetQuadratic(float newQuad) { quadratic = newQuad; }
    void EditLightComponent(LightComponent lc, glm::vec3 value);
    void EditLightComponent(LightComponent lc, glm::vec4 value);
    void EditLightComponent(LightComponent lc, float value);

    glm::vec3 inline GetPosition() { return this->position; }
    glm::vec3 inline GetDirection() { return this->direction; }
    glm::vec3 inline GetDiffuse() { return this->diffuse; }
    glm::vec3 inline GetSpecular() { return this->specular; }
    glm::vec3 inline GetAmbient() { return this->ambient; }
    TypeLight inline GetType() { return this->type; }
    float inline GetCutOff() { return this->cutOff; }
    float inline GetOuterCutOff() { return this->outerCutOff; }
    float inline GetConstant() { return this->constant; }
    float inline GetLinear() { return this->linear; }
    float inline GetQuadratic() { return this->quadratic; }
};

#endif
