#pragma once
#ifndef FBOSYSTEM_H
#define FBOSYSTEM_H

#include "FBO.h"

class FBOSystem
{
private:
    FBO* omniFBO;
    FBO* dirFBO;
    FBO* colorFBO;
    FBO* mrtFBO;
    FBO* pingpongFBO;
    int *width, *height;
public:
    FBOSystem(int *width, int *height);
    void AddFBO(FBO* fbo);
    void OmniShadowPass(unsigned int idPass);
    void DirShadowPass(unsigned int idPass);
    void PingPongPass(unsigned int idPass);
    void MRTPass();
    void FinalPass();
    void MultisamplingPass();
    unsigned int GetFinalRender();
    unsigned int GetMRTRender(int idTex = 0);
    unsigned int GetPingPongRender(int idTex = 0);
    unsigned int GetOmniRender(unsigned int id = 0);
    //unsigned int GetOmniRender();
	unsigned int GetDirRender(unsigned int id = 0);
    //unsigned int GetDirRender();
    void ResizeFBOs();
};

#endif
