#pragma once
#ifndef FBOSYSTEM_H
#define FBOSYSTEM_H

class FBOSystem
{
private:
    unsigned int framebuffer;
    unsigned int multiSampFramebuffer;
    unsigned int depthMapFBO;
    unsigned int depthCubeMapFBO;
    // generate texture
    bool isShadowMap;
    bool isShadowCubeMap;
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
    unsigned int texDepthCubeMap;
    FBOSystem(int *width, int *height, int numSamples = 0);
    void InitFBOSystem();
    void ActivateFBORender();
    void ActivateFBODepthMapRender();
    void ActivateFBODepthCubeMapRender();
    void SetMultiSamplingFrameBuffer();
    unsigned int &GetRenderedTexture() { return texColorBuffer; }
    void inline ActivateShadowMap() { isShadowMap = true; }
    void inline ActivateShadowCubeMap() { isShadowCubeMap = true; }
};

#endif
