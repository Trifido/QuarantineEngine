#pragma once
#ifndef FBOSYSTEM_H
#define FBOSYSTEM_H

class FBOSystem
{
private:
    unsigned int framebuffer;
    unsigned int multiSampFramebuffer;
    // generate texture
    
    //render buffer object
    unsigned int rbo;
    //screen size
    int *width, *height;
public:
    bool isAntiAliasing;
    unsigned int samples;
    unsigned int texColorBuffer;
    unsigned int textureColorBufferMultiSampled;
    FBOSystem(int *width, int *height, int numSamples = 0);
    void InitFBOSystem();
    void ActivateFBORender();
    void SetMultiSamplingFrameBuffer();
    unsigned int &GetRenderedTexture() { return texColorBuffer; }
};

#endif
