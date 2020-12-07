#pragma once

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Shader.h"
#include "Particle.h"
#include "Transform.h"
#include "Mesh.h"
#include <list>
#include <math.h>

enum ParticleSystemType
{
    COMMON_PS = 1,
    VOLUMEN_PS = 2,
    BILLBOARD = 3
};

struct ParticleLifeColor
{
    float percentageTime;
    glm::vec4 color;
    bool isColor;
    bool isAlpha;

    ParticleLifeColor()
    {
        percentageTime = 0.0f;
        color = glm::vec4(0.0f);
        isColor = false;
        isAlpha = false;
    }

    ParticleLifeColor(float time, glm::vec4 currentColor, bool isCurrentColor = false, bool isCurrentAlpha = false)
    {
        percentageTime = time;
        color = currentColor;
        isColor = isCurrentColor;
        isAlpha = isCurrentAlpha;
    }
};

struct VertexParticle
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
};

struct InstanceParticleAttr
{
    glm::vec4 textureOffset;
    glm::vec4 blendFactorRow; // 1->blend, 2->row, 3->col 4->isLifeColor-
    glm::mat4 modelInstance;
    glm::vec4 lifeColor;

    InstanceParticleAttr()
    {
        textureOffset = glm::vec4(0.0f);
        blendFactorRow = glm::vec4(0.0f);
        modelInstance = glm::mat4(1.0f);
        lifeColor = glm::vec4(0.0f);
    }
};

class ParticleSystem
{
private:
    ParticleSystemType type;
    unsigned int VAO, VBO, EBO;
    unsigned int instanceBuffer;
    std::list<Particle> particles;
    static const float particleVertex[];
    std::vector<VertexParticle> vertices;
    std::vector<unsigned int> indices;

    bool isColorLife;
    std::list<ParticleLifeColor> colorLife;

    Transform* transform;
    Camera* camera;
    Texture* particleTexture;
    float* deltaTime;
    float particlesPerSecond;
    float lifeLengthParticles;
    float gravityMass;
    float speed;
    float particleScale;
    float particleRotation;
    float emitRadius;
    float emitAngle;
    bool isInfinity = false;
    glm::vec3 systemCenter;
    int numRowTexture, numColTexture;

    int amount = 1000;
    InstanceParticleAttr* instancedAttributes;

    void UpdateParticleAttributes(int id, Particle& particleInstanced);
    std::list<Particle> MergeSort(std::list<Particle> mergeList);
    std::list<Particle> Merge(std::list<Particle> &left, std::list<Particle> &right);
public:
    Shader* particleShader;

    ParticleSystem();
    ParticleSystem(ParticleSystemType type, std::string nameTexture, int numRows = 1, int numCols = 1);
    void SetDeltaTime(float* delta);
    void Render();
    void AddCamera(Camera* mainCamera);
    void setupMesh();
    void setupInstanceMesh();
    void DeleteGPUInfo();
    void AddPlane(unsigned int numVertex);
    void GenerateParticles();
    void EmitParticle(glm::vec3 center);
    void setupTexture();
    void UpdatePosition();
    void UpdateRotation();
    void UpdateScale();
    void UpdateParticleSystem();

    inline float* GetParticlesCenterPosition() { return &systemCenter[0]; }
    inline float* GetParticlesRotation() { return &particleRotation; }
    inline float* GetParticlesPerSecond() { return &particlesPerSecond; }
    inline int* GetParticlesAmount() { return &amount; }
    inline float* GetParticlesLifeLength() { return &lifeLengthParticles; }
    inline float* GetParticlesGravityMass() { return &gravityMass; }
    inline float* GetParticlesSpeed() { return &speed; }
    inline float* GetParticlesScale() { return &particleScale; }
    inline bool* GetIsInfinityLife() { return &isInfinity; }
    inline float* GetEmitRadius() { return &emitRadius; }
    inline float* GetEmitAngle() { return &emitAngle; }
    inline ParticleSystemType GetType() { return type; }
    void SetPosition(glm::vec3 pos);
    void SetScale(float scal);
    void SetRotation(float rot);
    void SetProperties(float pps, float lifep, float speedp, float gravityp, float radius, float angle);
    void AddColorLife(ParticleLifeColor pColor);
    void UpdateColorLife(Particle &particle);
};

#endif
