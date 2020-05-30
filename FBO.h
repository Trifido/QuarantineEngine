#pragma once
#ifndef FBO_H
#define FBO_H
 
#include <vector>

enum FBOType
{
    COLOR_FBO,
    DIR_SHADOW_FBO,
    OMNI_SHADOW_FBO,
    MULT_RT,
    PINGPONG_FBO,
    DEFFERED
};

enum FBOComponent
{
    ANTIALIASING,
    SAMPLES,
    RESOLUTION_DEPTH,
    NUM_MRT,
    TYPE_FBO
};

class FBO
{
    private:
        bool isAntiAliasing;
        unsigned int *idFBO;
        unsigned int numFBOs; 
        unsigned int rbo;
        unsigned int samples; 
        unsigned int *texture_buffers;
        unsigned int num_mrt; 
        int *width, *height;
        int *widthDepthMap, *heightDepthMap;
        FBOType type;
        float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    public:
        FBO() {};
        FBO(FBOType type, unsigned int samples=1, unsigned int numTextures = 1);
        void GenerateFBO(int *width, int *height);
        void EditFBOComponent(FBOComponent fboComp, unsigned int value);
        void EditFBOComponent(FBOComponent fboComp, bool value);
        void EditFBOComponent(FBOComponent fboComp, int value);
        void EditFBOComponent(FBOComponent fboComp, FBOType value);
        FBOType GetType() { return type; }
        unsigned int GetRenderTexture(unsigned int id = 0);
        void ActivateFBO(unsigned int id = 0);
        void SetMultiSamplingFrameBuffer();
        void CheckFBO();
};

#endif
