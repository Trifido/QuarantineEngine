#pragma once

#ifndef PARTICLE_H
#define PARTICLE_H

#include "glm/glm.hpp"

class Particle
{
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float gravityEffect;
    float lifeLength;
    float rotation;
    float scale;
    float elapsedTime = 0.0f;
    float gravityValue = -9.8f;
    bool isRender = false;
    bool isInfinity = false;
    glm::vec2 currentOffSet;
    glm::vec2 nextOffSet;
    float blendTextValue;
    void SetTextureOffset(int numRows, int numCols, glm::vec2& offset, int index);
public:
    Particle();
    Particle(glm::vec3 pos, glm::vec3 vel, float gravity, float life, float rot, float scal, bool isInfinity = false);
    bool Update(float deltaTime);
    glm::vec3 GetPosition() { return position; }
    float GetRotation() { return rotation; }
    float GetScale() { return scale; }
    float GetElapsedTime() { return elapsedTime; }
    glm::vec2 GetCurrentOffset() { return currentOffSet; }
    glm::vec2 GetNextOffset() { return nextOffSet; }
    float GetBlendValueTextures() { return blendTextValue; }
    void UpdateTexture(int numRows, int numCols);
    inline void SetPostion(glm::vec3 pos) { position = pos; }
    inline void SetScale(float scal) { scale = scal; }
    inline void SetRotation(float rot) { rotation = rot; }
    inline glm::vec4* GetColor() { return &color; }
    inline void SetColor(glm::vec4 currentColor) { color = currentColor; }
};

#endif
