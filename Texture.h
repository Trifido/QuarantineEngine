#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl3w.h>



class Texture
{
private:
    int width, height, nrChannels;
public:
    unsigned int ID;

    Texture(char* imagePath, unsigned int wrap = GL_REPEAT, unsigned int filter = GL_LINEAR, bool flip = false);
    void LoadImage(char* imagePath, bool flip = false);
    
};

#endif
