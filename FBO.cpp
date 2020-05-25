#include "FBO.h"

#include <GL/gl3w.h>

FBO::FBO(FBOType type, unsigned int samples, unsigned int numTextures)
{
    this->type = type;
    this->isAntiAliasing = samples > 1;
    if (isAntiAliasing || type == FBOType::PINGPONG_FBO)
        numFBOs = 2;
    else if (numTextures > 1 && (type == FBOType::DIR_SHADOW_FBO || type == FBOType::OMNI_SHADOW_FBO))
        numFBOs = numTextures;
    else
        numFBOs = 1;

    idFBO = new unsigned int[numFBOs];

    for(int id = 0; id < numFBOs; id++)
        glGenFramebuffers(1, &idFBO[id]);

    if (isAntiAliasing || type == FBOType::MULT_RT || type == FBOType::PINGPONG_FBO)
    {
        this->num_mrt = 2;
        if (isAntiAliasing && type == FBOType::MULT_RT)
            this->num_mrt = 4;
    }
    else if (numTextures > 1 && (type == FBOType::DIR_SHADOW_FBO || type == FBOType::OMNI_SHADOW_FBO))
        this->num_mrt = numTextures;
    else
        this->num_mrt = 1;

    this->texture_buffers = new unsigned int[num_mrt];
    glGenTextures(num_mrt, texture_buffers);

    widthDepthMap = new int(1024);
    heightDepthMap = new int(1024);
    this->samples = samples;
}

void FBO::GenerateFBO(int *width, int *height)
{
    this->height = height;
    this->width = width;
    switch (type)
    {
    case FBOType::COLOR_FBO:
        if (this->isAntiAliasing)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, idFBO[0]);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_buffers[1]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, *width, *height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_buffers[1], 0);

            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, *width, *height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                fprintf(stderr, "ERROR::FRAMEBUFFER_MULTISAMPLING:: Framebuffer antialiasing is not complete!\n");

            // configure post-processing framebuffer 
            glBindFramebuffer(GL_FRAMEBUFFER, idFBO[1]);

            glBindTexture(GL_TEXTURE_2D, texture_buffers[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_buffers[0], 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                fprintf(stderr, "ERROR::FRAMEBUFFER_MULTISAMPLING:: Intermediate Framebuffer antialiasing is not complete!\n");
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, idFBO[0]);
            glBindTexture(GL_TEXTURE_2D, texture_buffers[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_buffers[0], 0);
            // attach it to currently bound framebuffer object
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *width, *height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        }
        //CheckFBO();
        break;
    case FBOType::DIR_SHADOW_FBO:
        // create depth texture
        for (int id = 0; id < numFBOs; id++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, idFBO[id]);
            glBindTexture(GL_TEXTURE_2D, texture_buffers[id]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, *widthDepthMap, *heightDepthMap, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_buffers[id], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                fprintf(stderr, "ERROR::FRAMEBUFFER_DIRSHADOW:: Framebuffer dirShadow is not complete!\n");
            }
        }
        break;
    case FBOType::OMNI_SHADOW_FBO:
        for (int id = 0; id < numFBOs; id++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, idFBO[id]);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture_buffers[id]);
            for (unsigned int i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, *widthDepthMap, *heightDepthMap, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glBindFramebuffer(GL_FRAMEBUFFER, idFBO[id]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_buffers[id], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        break;
    case FBOType::MULT_RT:
        { 
            if (this->isAntiAliasing)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, idFBO[0]);

                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_buffers[2]);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, *width, *height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_buffers[2], 0);
                 
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_buffers[3]);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, *width, *height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, texture_buffers[3], 0);

                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, *width, *height);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

                unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
                glDrawBuffers(2, attachments);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    fprintf(stderr, "ERROR::FRAMEBUFFER_MULTISAMPLING:: Framebuffer antialiasing is not complete!\n");

                // configure second post-processing framebuffer 
                glBindFramebuffer(GL_FRAMEBUFFER, idFBO[1]);

                //for (unsigned int i = 0; i < num_mrt-2; i++)
                //{
                //    glBindTexture(GL_TEXTURE_2D, texture_buffers[i]);
                //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, *width, *height, 0, GL_RGB, GL_FLOAT, NULL);
                //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                //    // attach texture to framebuffer
                //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture_buffers[i], 0);
                //}

                glBindTexture(GL_TEXTURE_2D, texture_buffers[0]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, *width, *height, 0, GL_RGB, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                // attach texture to framebuffer
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_buffers[0], 0);

                glBindTexture(GL_TEXTURE_2D, texture_buffers[1]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, *width, *height, 0, GL_RGB, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                // attach texture to framebuffer
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture_buffers[1], 0);

                unsigned int rbo2;
                glGenRenderbuffers(1, &rbo2);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, *width, *height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo2);

                //glDrawBuffers(2, attachments);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    fprintf(stderr, "ERROR::FRAMEBUFFER_MULTISAMPLING:: Intermediate Framebuffer antialiasing is not complete!\n");
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, idFBO[0]);
                for (unsigned int i = 0; i < num_mrt; i++)
                {
                    glBindTexture(GL_TEXTURE_2D, texture_buffers[i]);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, *width, *height, 0, GL_RGB, GL_FLOAT, NULL);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    // attach texture to framebuffer
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture_buffers[i], 0);
                }

                // create and attach depth buffer (renderbuffer) 
                glGenRenderbuffers(1, &rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, *width, *height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
                // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
                unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
                glDrawBuffers(2, attachments);
                // finally check if framebuffer is complete
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    fprintf(stderr, "ERROR::FRAMEBUFFER_MRT::Framebuffer is not complete!\n");
                }
            }
        }
        break;
    case FBOType::PINGPONG_FBO:
        for (unsigned int i = 0; i < numFBOs; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, idFBO[i]);
            glBindTexture(GL_TEXTURE_2D, texture_buffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, *width, *height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_buffers[i], 0);
            // also check if framebuffers are complete (no need for depth buffer)
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                fprintf(stderr, "ERROR::FRAMEBUFFER_PINGPONG::Framebuffer is not complete!\n");
            }
        }
        break;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::EditFBOComponent(FBOComponent fboComp, unsigned int value)
{
    switch (fboComp)
    {
    case FBOComponent::SAMPLES:
        samples = value;
        break;
    case FBOComponent::NUM_MRT:
        num_mrt = value;
        break;
    }
}

void FBO::EditFBOComponent(FBOComponent fboComp, bool value)
{
    switch (fboComp)
    {
    case FBOComponent::ANTIALIASING:
        isAntiAliasing = value;
        break;
    }
}

void FBO::EditFBOComponent(FBOComponent fboComp, int value)
{
    switch (fboComp)
    {
    case FBOComponent::RESOLUTION_DEPTH:
        if(widthDepthMap != nullptr)
            delete(widthDepthMap);
        if(heightDepthMap != nullptr)
            delete(heightDepthMap);
        widthDepthMap = heightDepthMap = new int(value);
        break;
    }
}

void FBO::EditFBOComponent(FBOComponent fboComp, FBOType value)
{
    type = value;
}

unsigned int FBO::GetRenderTexture(unsigned int id)
{
    return texture_buffers[id];
}

void FBO::ActivateFBO(unsigned int id)
{
    if(id < numFBOs)
        glBindFramebuffer(GL_FRAMEBUFFER, idFBO[id]);
}

void FBO::CheckFBO()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
}

void FBO::SetMultiSamplingFrameBuffer()
{
    if (isAntiAliasing)
    {
        if (this->GetType() == FBOType::MULT_RT)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, idFBO[0]);
            glReadBuffer(GL_COLOR_ATTACHMENT1);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, idFBO[1]);
            glDrawBuffer(GL_COLOR_ATTACHMENT1);

            glBlitFramebuffer(0, 0, *width, *height, 0, 0, *width, *height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, idFBO[0]);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, idFBO[1]);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);

            glBlitFramebuffer(0, 0, *width, *height, 0, 0, *width, *height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        else
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, idFBO[0]); 
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, idFBO[1]); 

            glBlitFramebuffer(0, 0, *width, *height, 0, 0, *width, *height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }
}
