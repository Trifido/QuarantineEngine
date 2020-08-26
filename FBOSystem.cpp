#include "FBOSystem.h"

#include <GL/gl3w.h>

FBOSystem::FBOSystem(int *width, int *height)
{
    this->width = width;
    this->height = height;
    colorFBO = nullptr;
    dirFBO = nullptr;
    omniFBO = nullptr;
    mrtFBO = nullptr;
    deferredFBO = nullptr;
    lightVolumeFBO = nullptr;
    ssaoFBO = nullptr;
    skyboxFBO = nullptr;
    prefilterFBO = nullptr;
}

FBO* FBOSystem::GetFBO(FBOType type)
{
    switch (type)
    {
    case FBOType::COLOR_FBO:
        return colorFBO;
    case FBOType::DEFFERED:
        return deferredFBO;
    case FBOType::DIR_SHADOW_FBO:
        return dirFBO;
    case FBOType::LIGHTING_VOLUME_FBO:
        return lightVolumeFBO;
    case FBOType::MULT_RT:
        return mrtFBO;
    case FBOType::OMNI_SHADOW_FBO:
        return omniFBO;
    case FBOType::PINGPONG_FBO:
        return pingpongFBO;
    case FBOType::SSAO_FBO:
        return ssaoFBO;
    case FBOType::SKYBOX_FBO:
        return skyboxFBO;
    case FBOType::PREFILTER_FBO:
        return prefilterFBO;
    }
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
    case FBOType::DEFFERED:
        deferredFBO = fbo;
        break;
    case FBOType::LIGHTING_VOLUME_FBO:
        lightVolumeFBO = fbo;
        break;
    case FBOType::SSAO_FBO:
        ssaoFBO = fbo;
        break;
    case FBOType::SKYBOX_FBO:
        skyboxFBO = fbo;
        break;
    case FBOType::PREFILTER_FBO:
        prefilterFBO = fbo;
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

void FBOSystem::MRTPass(unsigned int idFBO)
{
    if(mrtFBO != nullptr)
        mrtFBO->ActivateFBO(idFBO);
}

void FBOSystem::SSAOPass(unsigned int idFBO)
{
    if (ssaoFBO != nullptr)
        ssaoFBO->ActivateFBO(idFBO);
}

void FBOSystem::FinalPass()
{
    if(colorFBO != nullptr)
        colorFBO->ActivateFBO();
}

void FBOSystem::DeferredGeometryPass()
{
    if (deferredFBO != nullptr)
        deferredFBO->ActivateFBO();
}

void FBOSystem::LightVolumePass()
{
    if (lightVolumeFBO != nullptr)
        lightVolumeFBO->ActivateFBO();
}

void FBOSystem::MultisamplingPass()
{
    if(colorFBO != nullptr)
        colorFBO->SetMultiSamplingFrameBuffer();
    else if (mrtFBO != nullptr)
        mrtFBO->SetMultiSamplingFrameBuffer();
}

void FBOSystem::SkyboxProcessPass()
{
    if (skyboxFBO != nullptr)
        skyboxFBO->ActivateFBO();
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

unsigned int FBOSystem::GetOmniRender(unsigned int id)
{
    if(omniFBO != nullptr)
        return omniFBO->GetRenderTexture(id);
    return 0;
}

unsigned int FBOSystem::GetDeferredRender(unsigned int id)
{
    if (dirFBO != nullptr)
        return deferredFBO->GetRenderTexture(id);
    return 0;
}

unsigned int FBOSystem::GetLightVolumeRender(unsigned int id)
{
    if (lightVolumeFBO != nullptr)
        return lightVolumeFBO->GetRenderTexture(id);
    return 0;
}

unsigned int FBOSystem::GetSSAORender(unsigned int id)
{
    if (ssaoFBO != nullptr)
        return ssaoFBO->GetRenderTexture(id);
    return 0;
}

unsigned int FBOSystem::GetSkyboxRender(unsigned int id)
{
	if (skyboxFBO != nullptr)
		return skyboxFBO->GetRenderTexture(id);
	return 0;
}

unsigned int FBOSystem::GetDirRender(unsigned int id)
{
    if(dirFBO != nullptr)
        return dirFBO->GetRenderTexture(id);
    return 0;
}

unsigned int FBOSystem::GetPrefilterRender(unsigned int id)
{
    if(prefilterFBO != nullptr)
        return prefilterFBO->GetRenderTexture(id);
    return 0;
}

void FBOSystem::ResizeFBOs()
{
    if (colorFBO != nullptr)
        colorFBO->GenerateFBO(width, height);
    if (mrtFBO != nullptr)
        mrtFBO->GenerateFBO(width, height);
    if (pingpongFBO != nullptr)
        pingpongFBO->GenerateFBO(width, height);
    if (deferredFBO != nullptr)
        deferredFBO->GenerateFBO(width, height);
    if (lightVolumeFBO != nullptr)
        lightVolumeFBO->GenerateFBO(width, height);
    if (ssaoFBO != nullptr)
        ssaoFBO->GenerateFBO(width, height);
}

void FBOSystem::BlitDepthBuffer(FBOType readBuffer, FBOType drawBuffer)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GetFBO(readBuffer)->GetID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GetFBO(drawBuffer)->GetID());

    glBlitFramebuffer(0, 0, *width, *height, 0, 0, *width, *height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBOSystem::SetIrradianceMap()
{
    if (skyboxFBO != nullptr)
    {
        skyboxFBO->SetIrradianceCubemap();
    }
}

void FBOSystem::UpdateMRT(unsigned int numSamples)
{
    if (mrtFBO != nullptr)
    {
        delete mrtFBO;

        AddFBO(new FBO(FBOType::MULT_RT, numSamples));
    }
}

void FBOSystem::UpdateDirShadowFbo(unsigned int numRTargets)
{
    if (dirFBO != nullptr)
    {
        delete dirFBO;
        AddFBO(new FBO(FBOType::DIR_SHADOW_FBO, 0, numRTargets));
    }
}

void FBOSystem::UpdateOmniShadowFbo(unsigned int numRTargets)
{
    if (omniFBO != nullptr)
    {
        delete omniFBO;
        AddFBO(new FBO(FBOType::OMNI_SHADOW_FBO, 0, numRTargets));
    }
}
