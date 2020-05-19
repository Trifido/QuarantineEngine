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
    OUTLINE,
    NORMALS,
    PROCEDURAL,
    INSTANCE
};

enum MaterialComponent
{
    TYPE,
    SHADER1,
    SHADER2,
    SHADER_SHADOW,
    SHADER_POINT_SHADOW,
    TEXTURE,
    SHININESS,
    P_DISPLACEMENT,
    BLINN,
    NUM_INSTANCES,
    REFRACTIVE_INDEX,
    OUTLINE_COLOR,
    A_REFLECTIVE,
    A_REFRACTIVE,
    DRAW_MODE
};

enum DrawMode
{
    DPOINTS,
    DLINES,
    DLINES_STRIP,
    DTRIANGLES,
    DTRIANGLES_STRIP
};

class Material
{
private:
    unsigned int numTextures = 0;
public:
    MaterialType type;
    Shader* ptrShader;
    Shader* ptrShader2;
    Shader* ptrShaderShadow;
    Shader* ptrShaderPointShadow;
    std::vector<Texture> textures;
    Texture* skyboxTexture;
    float shininess;
    float refractiveIndex;
    float parallax_displacement;
    bool isAmbientReflective = false;
    bool isAmbientRefractive = false;
    bool isBlinnShading = true;
    glm::vec4 colorOutline;
    DrawMode drawtype;
    int numInstances;

    Material();
    Material(Shader* shader);
    Material(Shader* shader, Shader* shader2, MaterialType mattype = MaterialType::LIT);
    Material(Shader* shader, std::vector<Texture> textures);
    Material(Shader* shader, Shader* shader2, std::vector<Texture> textures);
    void CopyMaterial(Material mat);

    void AddShader(Shader* sh);
    void AddTexture(Texture texture);
	void ActivateShadowTexture(unsigned int idTexShadow, int idLight, TypeLight type = TypeLight::DIRL);
    //void ActivateShadowTexture(unsigned int idTexShadow, bool isOmni = false);
    void AddMultTextures(std::vector<Texture> texturesIN);
    void AssignRenderTextures();
};

#endif
