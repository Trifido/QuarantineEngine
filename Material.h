#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include "Texture.h"

enum MaterialType
{
    UNLIT,
    LIT,
    TRANSP,
    OUTLINE
};

enum MaterialComponent
{
    TYPE,
    SHADER1,
    SHADER2,
    TEXTURE,
    SHININESS,
    REFRACTIVE_INDEX,
    OUTLINE_COLOR,
    A_REFLECTIVE,
    A_REFRACTIVE
};

class Material
{
public:
    MaterialType type;
    Shader* ptrShader;
    Shader* ptrShader2;
    std::vector<Texture> textures;
    Texture* skyboxTexture;
    float shininess;
    float refractiveIndex;
    bool isAmbientReflective = false;
    bool isAmbientRefractive = false;
    glm::vec4 colorOutline;

    Material();
    Material(Shader* shader);
    Material(Shader* shader, std::vector<Texture> textures);
    Material(Shader* shader, Shader* shader2, std::vector<Texture> textures);

    void AddShader(Shader* sh);
    void AddTexture(Texture texture);
    void AddMultTextures(std::vector<Texture> texturesIN);
    void AssignRenderTextures();
};

#endif
