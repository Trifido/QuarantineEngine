#pragma once
#ifndef FBOSYSTEM_H
#define FBOSYSTEM_H

class FBOSystem
{
private:
    unsigned int framebuffer;
    // generate texture
    
    //render buffer object
    unsigned int rbo;
    //screen size
    int *width, *height;
public:
    unsigned int texColorBuffer;
    FBOSystem(int *width, int *height);
    void InitFBOSystem();
    void ActivateFBORender();
    unsigned int &GetRenderedTexture() { return texColorBuffer; }
};

#endif
