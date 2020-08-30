#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include "HeadersRequeriments.h"

enum TextureComponent
{
    TEX_GAMMA,
    TEX_WIDTH,
    TEX_HEIGHT,
    TEX_CHANNELS,
    TEX_PATH,
    TEX_TYPE
};

enum TypeTexture
{
    DIFFUSE = 0,
    SPECULAR = 1,
    NORMAL = 2,
    HEIGHT = 3,
    EMISSIVE = 4,
    CUBEMAP = 5,
    AO = 6,
    ROUGHNESS = 7,
    METALLIC = 8,
    BUMP = 9,
    HDR_SKYBOX = 10,
    NOISE = 11
};

class Texture
{
private:
    int width, height, nrChannels; 
    unsigned int wrap, filter;
    bool isGammaTexture;
    bool flip;
    std::string imgPath;
public:
    unsigned int ID;
    std::string path;
    TypeTexture type;

    Texture() {};
    ~Texture();
    Texture(std::string imagePath, bool isGamma = false, unsigned int wrap = GL_REPEAT, unsigned int filter = GL_LINEAR, bool flip = false);
    Texture(std::string imagePath, TypeTexture type, bool isGamma = false, unsigned int wrap = GL_REPEAT, unsigned int filter = GL_LINEAR, bool flip = false);
    Texture(std::vector<std::string> imagePath, TypeTexture type, bool isGamma = false, unsigned int wrap = GL_CLAMP_TO_EDGE, unsigned int filter = GL_LINEAR, bool flip = false);
    void EditComponet(TextureComponent component, bool value);
    void EditComponet(TextureComponent component, int value);
    void EditComponet(TextureComponent component, std::string value);
    void EditComponet(TextureComponent component, TypeTexture value);

    unsigned int GetIDTexture() { return ID; }
    std::string GetPathTexture() { return path; }
    unsigned int RawTypeTexture();
    void SetRawTypeTexture(unsigned int id);
    const char* GetRawPath() { return imgPath.c_str(); };

private:
    void LoadTextureObj();
    void LoadTextureHDR();
    void LoadImage(const char* imagesPath, bool flip = false);
    void LoadHDRImage(const char* imagesPath, bool flip = false);
};

#endif
