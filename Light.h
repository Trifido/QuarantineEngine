#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "libs/glm/glm.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include "libs/glm/gtc/type_ptr.hpp"

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
public:
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
