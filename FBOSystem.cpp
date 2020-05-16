#include "FBOSystem.h"

FBOSystem::FBOSystem(int *width, int *height)
{
    this->width = width;
    this->height = height;
    colorFBO = nullptr;
    dirFBO = nullptr;
    omniFBO = nullptr;
    mrtFBO = nullptr;
}

void FBOSystem::AddFBO(FBO* fbo)
{
    fbo->GenerateFBO(width, height);

    switch (fbo->GetType())
    {
    case FBOType::COLOR_FBO:
        colorFBO = fbo;
        break;
    case FBOType::DIR_SHADOW_FBO:
        dirFBO = fbo;
        break;
    case FBOType::OMNI_SHADOW_FBO:
        omniFBO = fbo;
        break;
    case FBOType::MULT_RT:
        mrtFBO = fbo;
        break;
    case FBOType::PINGPONG_FBO:
        pingpongFBO = fbo;
        break;
    } 
}

void FBOSystem::OmniShadowPass(unsigned int idPass)
{
    if(omniFBO != nullptr)
        omniFBO->ActivateFBO(idPass);
}

void FBOSystem::DirShadowPass(unsigned int idPass)
{
    if(dirFBO != nullptr)
        dirFBO->ActivateFBO(idPass);
}

void FBOSystem::PingPongPass(unsigned int idPass)
{
    if (pingpongFBO != nullptr)
        pingpongFBO->ActivateFBO(idPass);
}

void FBOSystem::MRTPass()
{
    if(mrtFBO != nullptr)
        mrtFBO->ActivateFBO();
}

void FBOSystem::FinalPass()
{
    if(colorFBO != nullptr)
        colorFBO->ActivateFBO();
}

void FBOSystem::MultisamplingPass()
{
    if(colorFBO != nullptr)
        colorFBO->SetMultiSamplingFrameBuffer();
}

unsigned int FBOSystem::GetFinalRender()
{
    if(colorFBO != nullptr)
        return colorFBO->GetRenderTexture();
    return 0;
}

unsigned int FBOSystem::GetMRTRender(int idTex)
{
    if (mrtFBO != nullptr)
        return mrtFBO->GetRenderTexture(idTex);
    return 0;
}

unsigned int FBOSystem::GetPingPongRender(int idTex)
{
    if(pingpongFBO != nullptr)
        return pingpongFBO->GetRenderTexture(idTex);
    return 0;
}

unsigned int FBOSystem::GetOmniRender()
{
    if(omniFBO != nullptr)
        return omniFBO->GetRenderTexture();
    return 0;
}

unsigned int FBOSystem::GetDirRender()
{
    if(dirFBO != nullptr)
        return dirFBO->GetRenderTexture();
    return 0;
}

void FBOSystem::ResizeFBOs()
{
    if(colorFBO != nullptr)
        colorFBO->GenerateFBO(width, height);
}
