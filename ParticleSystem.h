#pragma once

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Shader.h"
#include "Particle.h"
#include "Transform.h"
#include "Mesh.h"
#include <list>
#include <math.h>

struct VertexParticle
{
    glm::vec3 Position;
    glm::vec2 TexCoords;
};

class ParticleSystem
{
private:
    unsigned int VAO, VBO, EBO;
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

    void UpdateTransformation(glm::vec3 position, float rotation, float scale);
    void UpdateTextureInfo(Particle& particle);
    std::list<Particle> MergeSort(std::list<Particle> mergeList);
    std::list<Particle> Merge(std::list<Particle> &left, std::list<Particle> &right);
public:
    Shader* particleShader;

    ParticleSystem();
    void SetDeltaTime(float* delta);
    void Render();
    void AddCamera(Camera* mainCamera);
    void setupMesh();
    void DeleteGPUInfo();
    void AddPlane(unsigned int numVertex);
    void GenerateParticles();
    void EmitParticle(glm::vec3 center);
    void setupTexture();
};

#endif
