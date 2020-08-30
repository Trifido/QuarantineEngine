#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include "Texture.h"

enum MaterialType
{
    UNLIT = 0,
    LIT = 1,
    TRANSP = 2,
    OUTLINE = 3,
    NORMALS = 4,
    PROCEDURAL = 5,
    INSTANCE = 6,
    EMISSIVE_LIT = 7,
    BOUNDING_VOLUME = 8,
    FPS = 9,
    INTERNAL = 10
};

enum MaterialComponent
{
    TYPE,
    SHADER1,
    SHADER2,
    SHADER_FORWARD,
    SHADER_DEFERRED,
    SHADER_SHADOW,
    SHADER_POINT_SHADOW,
    TEXTURE,
    MIN_UV,
    MAX_UV,
    SHININESS,
    P_DISPLACEMENT,
    BLINN,
    NUM_INSTANCES,
    REFRACTIVE_INDEX,
    OUTLINE_COLOR,
    A_REFLECTIVE,
    A_REFRACTIVE,
    DRAW_MODE,
    RENDER_MODE,
    BLOOM_BRIGHTNESS
};

enum DrawMode
{
    DPOINTS = 0,
    DLINES = 1,
    DLINES_STRIP = 2,
    DTRIANGLES = 3,
    DTRIANGLES_STRIP = 4
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
    float min_uv, max_uv;
    float bloomBrightness;
    bool isAmbientReflective = false;
    bool isAmbientRefractive = false;
    bool isBlinnShading = true;
    bool isBounding = false;
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
	void ActivateIrradianceMap(unsigned int idTexIrradiance, unsigned int idTexPrefilter, unsigned int idTexBrdf);
    void AddMultTextures(std::vector<Texture> texturesIN);
    void AssignRenderTextures();

    unsigned int RawMaterialType() { return type; }
    unsigned int RawDrawType() { return drawtype; }
    void SetRawMaterialType(unsigned int id) { type = (MaterialType)id; }
    void SetRawDrawType(unsigned int id) { drawtype = (DrawMode)id; }
    float* RawShininess() { return &shininess; }
    float* RawRefractiveIndex() { return &refractiveIndex; }
    float* RawParallaxDisplacement() { return &parallax_displacement; }
    float* RawMinUV() { return &min_uv; }
    float* RawMaxUV() { return &max_uv; }
    bool* RawIsAmbientReflective() { return &isAmbientReflective; }
    bool* RawIsAmbientRefractive() { return &isAmbientRefractive; }
    bool* RawIsBlinnShading() { return &isBlinnShading; }
    float* RawColorOutline() { return &colorOutline[0]; }
};

#endif
