#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "HeadersRequeriments.h"

enum ShadowType
{
    SHADOW_MAP = 0,
    SHADOW_VOL = 1,
    SHADOW_CAS = 2
};

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
    LIGHT_PROJSHADOW,
    LIGHT_ASPECT,
    LIGHT_CAST_SHADOW,
    LIGHT_CUTOFF,
    LIGHT_OUTERCUTOFF
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
    float radius;

    TypeLight type;

    float near_plane;
    float far_plane;
    glm::vec4 projDimension;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    float aspect;
public:
    bool isCastShadow;
    bool isShadowMap;
    bool isVolumeShadow;
    bool isCascadeShadow;

    glm::mat4 lightSpaceMatrix;
    std::vector<glm::mat4> lightSpaceMatrices;
    Light();
    Light(TypeLight newType);
    Light(TypeLight nType, glm::vec3 newPos);
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
    void EditLightComponent(LightComponent lc, bool value);

    glm::vec3 inline GetPosition() { return this->position; }
    float* GetRawPosition();
    glm::vec3 inline GetDirection() { return this->direction; }
    float* GetRawDirection();
    glm::vec3 inline GetDiffuse() { return this->diffuse; }
    float* GetRawDiffuse();
    glm::vec3 inline GetSpecular() { return this->specular; }
    float* GetRawSpecular();
    glm::vec3 inline GetAmbient() { return this->ambient; }
    float* GetRawAmbient();
    TypeLight inline GetType() { return this->type; }
    float inline GetCutOff() { return this->cutOff; }
    float* GetRawCutOff() { return &cutOff; }
    float inline GetOuterCutOff() { return this->outerCutOff; }
    float* GetRawOuterCutOff() { return &outerCutOff; }
    float inline GetConstant() { return this->constant; }
    float inline GetLinear() { return this->linear; }
    float* GetRawLinear() { return &linear; }
    float inline GetQuadratic() { return this->quadratic; }
    float* GetRawQuadratic() { return &quadratic; }
    float inline GetFarplane() { return this->far_plane; }
    float inline GetRadius() { return this->radius; }
    bool* GetRawCastShadow() { return &isCastShadow; }
    void SetShadowCastMode(ShadowType mode);
    int GetShadowMode();
private:
    void ComputeShadowProjection();
    void ComputeShadowCubeMapProjection();
    void Init();
};

#endif
