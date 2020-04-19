#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include "HeadersRequeriments.h"



enum TypeTexture
{
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
    EMISSIVE,
    CUBEMAP
};

class Texture
{
private:
    int width, height, nrChannels;
public:
    unsigned int ID;
    std::string path;
    TypeTexture type;

    Texture() {};
    Texture(std::string imagePath, unsigned int wrap = GL_REPEAT, unsigned int filter = GL_LINEAR, bool flip = false);
    Texture(std::string imagePath, TypeTexture type, unsigned int wrap = GL_REPEAT, unsigned int filter = GL_LINEAR, bool flip = false);
    Texture(std::vector<std::string> imagePath, TypeTexture type, unsigned int wrap = GL_CLAMP_TO_EDGE, unsigned int filter = GL_LINEAR, bool flip = false);
    void LoadImage(const char* imagesPath, bool flip = false);
    
};

#endif
