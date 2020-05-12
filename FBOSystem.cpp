#include "FBOSystem.h"
#include <stdio.h>
#include <GL/gl3w.h>



FBOSystem::FBOSystem(int *width, int *height, int numSamples)
{
    this->width = width;
    this->height = height;
    this->isAntiAliasing = (numSamples > 1);

    if (this->isAntiAliasing)
        this->samples = numSamples;
    else
        this->samples = 1;

    this->widthDepthMap = new int(1024);
    this->heightDepthMap = new int(1024);
    this->isShadowMap = false;
    this->isShadowCubeMap = false;
}

void FBOSystem::InitFBOSystem()
{
    if (isShadowMap)
    {
        glGenFramebuffers(1, &depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        // create depth texture
        glGenTextures(1, &texDepthMap);
        glBindTexture(GL_TEXTURE_2D, texDepthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, *widthDepthMap, *heightDepthMap, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texDepthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            fprintf(stderr, "ERROR::FRAMEBUFFER_MULTISAMPLING:: Framebuffer antialiasing is not complete!\n");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else if (isShadowCubeMap)
    {
        glGenFramebuffers(1, &depthCubeMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);

        glGenTextures(1, &texDepthCubeMap); 
        glBindTexture(GL_TEXTURE_CUBE_MAP, texDepthCubeMap);

        for (unsigned int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, *widthDepthMap, *heightDepthMap, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texDepthCubeMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        if (isAntiAliasing)
        {
            glGenTextures(1, &textureColorBufferMultiSampled);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, *width, *height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, *width, *height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                fprintf(stderr, "ERROR::FRAMEBUFFER_MULTISAMPLING:: Framebuffer antialiasing is not complete!\n");

            // configure second post-processing framebuffer 
            glGenFramebuffers(1, &multiSampFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, multiSampFramebuffer);

            // create a color attachment texture 
            glGenTextures(1, &texColorBuffer);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                fprintf(stderr, "ERROR::FRAMEBUFFER_MULTISAMPLING:: Intermediate Framebuffer antialiasing is not complete!\n");
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        else
        {
            // generate texture
            glGenTextures(1, &texColorBuffer);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

            // attach it to currently bound framebuffer object
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *width, *height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                fprintf(stderr, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
}

void FBOSystem::ActivateFBORender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void FBOSystem::ActivateFBODepthMapRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
}

void FBOSystem::ActivateFBODepthCubeMapRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
}

void FBOSystem::SetMultiSamplingFrameBuffer()
{
    if (isAntiAliasing)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, multiSampFramebuffer);
        glBlitFramebuffer(0, 0, *width, *height, 0, 0, *width, *height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}
