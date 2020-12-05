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

struct VertexParticle
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
};

struct InstanceParticleAttr
{
    glm::vec4 textureOffset;
    glm::vec2 blendFactorRow;
    glm::mat4 modelInstance;

    InstanceParticleAttr()
    {
        textureOffset = glm::vec4(0.0f);
        blendFactorRow = glm::vec2(0.0f);
        modelInstance = glm::mat4(1.0f);
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

    Transform* transform;
    Camera* camera;
    Texture* particleTexture;
    float* deltaTime;
    float particlesPerSecond;
    float lifeLengthParticles;
    float gravityMass;
    float speed;
    glm::vec3 systemCenter;
    int numRowTexture;

    unsigned int amount = 100;// 000;// 00;// 100000;
    InstanceParticleAttr* instancedAttributes;

    void UpdateParticleAttributes(int id, Particle& particleInstanced);
    std::list<Particle> MergeSort(std::list<Particle> mergeList);
    std::list<Particle> Merge(std::list<Particle> &left, std::list<Particle> &right);
public:
    Shader* particleShader;

    ParticleSystem();
    ParticleSystem(ParticleSystemType type, std::string nameTexture, int numRows = 1);
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
};

#endif
