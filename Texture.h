#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl3w.h>
#include <string>

enum TypeTexture
{
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
    EMISSIVE
};

class Texture
{
private:
    int width, height, nrChannels;
public:
    unsigned int ID;
    std::string path;
    TypeTexture type;

    Texture(std::string imagePath, unsigned int wrap = GL_REPEAT, unsigned int filter = GL_LINEAR, bool flip = false);
    Texture(std::string imagePath, TypeTexture type, unsigned int wrap = GL_REPEAT, unsigned int filter = GL_LINEAR, bool flip = false);
    void LoadImage(const char* imagePath, bool flip = false);
    
};

#endif
