#pragma once

#ifndef PROCEDURAL_TEXTURE_H
#define PROCEDURAL_TEXTURE_H
#include "Texture.h"
#include "HeadersRequeriments.h"
#include <random>
#include <time.h>
#include <numeric>

enum NoiseType
{
    PERLIN = 0,
    WORLEY = 1,
    ALL = 2
};

class ProceduralTexture 
{
private:
    // The permutation vector
    //std::vector<int> p;
    int *p;
    unsigned int seed;
    unsigned int numBump;
    unsigned int tamTexture, nrChannels;
    unsigned char* rawTexture;
    int repeat;

    void GeneratePermutation();
    void ComputePerlin(unsigned int seed = 0);
    void ComputeWorley(unsigned int numBump = 5);
    double Noise(double x, double y, double z);
    void MapGradient(std::vector<std::vector<float>> noise);
    float WorleyNoise(glm::vec3 input, std::vector<glm::vec3> points, int size, float* maxDist, float* minDist);
    glm::vec2 random2(glm::vec2 p);
    float WrapDist(glm::vec3 pixel, glm::vec3 point);
    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);
    GLenum GetFormat();

    double perlin(double x, double y, double z);
    double OctavePerlin(double x, double y, double z, int octaves, double persistence);
    int inc(int num);
public:
    unsigned int ID;
    NoiseType typeNoise;
    ProceduralTexture();
    ProceduralTexture(NoiseType type, unsigned int tamTexture, unsigned int nrChannels = 3, unsigned int numRandomPoints = 237);
};

#endif
