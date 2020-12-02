#pragma once

#ifndef PARTICLE_H
#define PARTICLE_H

#include "glm/glm.hpp"

class Particle
{
    glm::vec3 position;
    glm::vec3 velocity;
    float gravityEffect;
    float lifeLength;
    float rotation;
    float scale;
    float elapsedTime = 0.0f;
    float gravityValue = -9.8f;
    bool isRender = false;
    glm::vec2 currentOffSet;
    glm::vec2 nextOffSet;
    float blendTextValue;
    void SetTextureOffset(int numRows, glm::vec2& offset, int index);
public:
    Particle();
    Particle(glm::vec3 pos, glm::vec3 vel, float gravity, float life, float rot, float scal);
    bool Update(float deltaTime);
    glm::vec3 GetPosition() { return position; }
    float GetRotation() { return rotation; }
    float GetScale() { return scale; }
    float GetElapsedTime() { return elapsedTime; }
    glm::vec2 GetCurrentOffset() { return currentOffSet; }
    glm::vec2 GetNextOffset() { return nextOffSet; }
    float GetBlendValueTextures() { return blendTextValue; }
    void UpdateTexture(int numRows);
};

#endif
