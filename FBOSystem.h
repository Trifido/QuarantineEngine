#pragma once
#ifndef FBOSYSTEM_H
#define FBOSYSTEM_H

class FBOSystem
{
private:
    unsigned int framebuffer;
    unsigned int multiSampFramebuffer;
    unsigned int depthMapFBO;
    // generate texture
    bool isShadowMap;
    //render buffer object
    unsigned int rbo;
    //screen size
    int *width, *height;
public:
    int* widthDepthMap, * heightDepthMap;
    bool isAntiAliasing;
    unsigned int samples;
    unsigned int texColorBuffer;
    unsigned int textureColorBufferMultiSampled;
    unsigned int texDepthMap;
    FBOSystem(int *width, int *height, int numSamples = 0);
    void InitFBOSystem();
    void ActivateFBORender();
    void ActivateFBODepthMapRender();
    void SetMultiSamplingFrameBuffer();
    unsigned int &GetRenderedTexture() { return texColorBuffer; }
    void inline ActivateShadowMap() { isShadowMap = true; }
};

#endif
