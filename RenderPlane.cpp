#include "RenderPlane.h"

float RenderPlane::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

RenderPlane::RenderPlane()
{
    gammaValue = 1.0f;
    exposureValue = 1.0f;
    amountBloom = 10;
    pingpongIdPass = true;
    screenRenderShader = new Shader("shaders/renderPass.vert", "shaders/renderPass.frag");
    shaderBlur = new Shader("shaders/renderPass.vert", "shaders/blurPass.frag");
    shaderBloomFinal = new Shader("shaders/renderPass.vert", "shaders/finalBloom.frag");
    deferredLighting = new Shader("shaders/deferredLighting.vert", "shaders/deferredLighting.frag");
    ssao = new Shader("shaders/deferredLighting.vert", "shaders/ssao.frag");
    ssao_blur = new Shader("shaders/deferredLighting.vert", "shaders/ssao_blur.frag");
    rayMarching = new Shader("shaders/rmSky.vert", "shaders/rmSky.frag");
    screenRenderVolumeShadow = new Shader("shaders/renderPass.vert", "shaders/renderPassVolume.frag");
    //screenRenderShader = new Shader("shaders/depthRenderShadow.vert", "shaders/depthRenderShadow.frag");
}

void RenderPlane::SetVAORenderPlane()
{
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    screenRenderShader->use();
    screenRenderShader->setInt("screenTexture", 3);
    screenRenderShader->setFloat("gamma", hdrGui->gammaParameter);
    screenRenderShader->setFloat("exposure", exposureValue);

    shaderBlur->use();
    shaderBlur->setInt("image", 0);

    shaderBloomFinal->use(); 
    shaderBloomFinal->setInt("bloom", bloomGui->enable);
    shaderBloomFinal->setFloat("gamma", hdrGui->gammaParameter);
    shaderBloomFinal->setFloat("exposure", exposureValue); 
    shaderBloomFinal->setInt("scene", 0);
    shaderBloomFinal->setInt("bloomBlur", 1);

    deferredLighting->use();
    deferredLighting->setInt("gPosition", 0);
    deferredLighting->setInt("gNormal", 1);
    deferredLighting->setInt("gAlbedo", 2);
    deferredLighting->setInt("gSpecular", 3);
    deferredLighting->setInt("gEmissive", 4);
    deferredLighting->setInt("ssao", 5);
    deferredLighting->setFloat("generalAmbient", 0.4f);
    deferredLighting->setFloat("gamma", hdrGui->gammaParameter);
    deferredLighting->setFloat("exposure", exposureValue);

    ssao->use();
    ssao->setInt("gPosition", 0);
    ssao->setInt("gNormal", 1);
    ssao->setInt("texNoise", 2);

    ssao_blur->use();
    ssao_blur->setInt("ssaoInput", 0);

    rayMarching->use();
    rayMarching->setInt("screenTexture", 3);

    rayMarching->setFloat("gamma", hdrGui->gammaParameter);
    rayMarching->setFloat("exposure", exposureValue);
    rayMarching->setVec2("resolution", glm::vec2(cam->WIDTH, cam->HEIGHT));
    rayMarching->setVec3("front", cam->cameraFront);
    rayMarching->setFloat("fov", cam->GetFOV());
}

void RenderPlane::FinalRenderPass()
{
    screenRenderShader->use();
    screenRenderShader->setFloat("gamma", hdrGui->gammaParameter);
    screenRenderShader->setFloat("exposure", hdrGui->exposureParameter);

    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetFinalRender());	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::FinalRenderBloom()
{
    shaderBloomFinal->use();
    shaderBloomFinal->setInt("bloom", bloomGui->enable && bloomGui->blurRange > 0);
    shaderBloomFinal->setFloat("gamma", hdrGui->gammaParameter);
    shaderBloomFinal->setFloat("exposure", hdrGui->exposureParameter);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetMRTRender(0)); //fboSystem->GetDirRender());//
    if (bloomGui->enable)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fboSystem->GetPingPongRender(!pingpongIdPass));
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::FinalRenderShadowVolume()
{
    screenRenderVolumeShadow->use();
    screenRenderVolumeShadow->setFloat("gamma", hdrGui->gammaParameter);
    screenRenderVolumeShadow->setFloat("exposure", hdrGui->exposureParameter);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetVolumeShadowRender(0));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::FinalRenderRayMarching()
{
    glBindVertexArray(quadVAO);
    rayMarching->use();
    rayMarching->ActivateCamera();

    rayMarching->setMat4("view", cam->view);
    rayMarching->setVec3("front", cam->cameraFront);
    rayMarching->setMat4("projection", cam->projection);
    rayMarching->setVec3("viewPos", cam->cameraPos);
    rayMarching->setFloat("gamma", hdrGui->gammaParameter);
    rayMarching->setFloat("exposure", hdrGui->exposureParameter);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetMRTRender(0)); //fboSystem->GetDirRender());//
    if (bloomGui->enable)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fboSystem->GetPingPongRender(!pingpongIdPass));
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::RenderBlur()
{
    if (bloomGui->enable)
    {
        bool first_iteration = true;
        shaderBlur->use();
        for (unsigned int i = 0; i < bloomGui->blurRange; i++)
        {
            fboSystem->PingPongPass(pingpongIdPass);
            shaderBlur->setInt("horizontal", pingpongIdPass);
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? fboSystem->GetMRTRender(1) : fboSystem->GetPingPongRender(!pingpongIdPass));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            pingpongIdPass = !pingpongIdPass;
            if (first_iteration)
                first_iteration = false;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPlane::DefferedRender()
{
    deferredLighting->use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetDeferredRender(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetDeferredRender(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetDeferredRender(2));
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetDeferredRender(3));
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetDeferredRender(4));
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetSSAORender(1));

    deferredLighting->ActivateCamera();
    deferredLighting->ActivateLights();

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::RenderSSAO()
{
    ssao->use();
    for (unsigned int i = 0; i < 64; ++i)
        ssao->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);

    ssao->setMat4("proj", cam->projection);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetDeferredRender(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetDeferredRender(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    //ssao->ActivateCamera();
    //ssao->ActivateLights();

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::RenderBlurSSAO()
{
    ssao_blur->use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetSSAORender());
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::AddLights(std::vector<Light*> lights)
{
    deferredLighting->AddLight(lights);
}

void RenderPlane::AddCamera(Camera* cam)
{
    this->cam = cam;
    deferredLighting->AddCamera(cam);
    rayMarching->AddCamera(cam);
}

void RenderPlane::GenerateNoiseTexture()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;

    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void RenderPlane::AddUBOSystem(UBOSystem* ubo)
{
    ubo->SetUniformBlockIndex(ssao->ID, "Matrices");
}

void RenderPlane::SetBloomParameters(BloomGUI* bloomParameters)
{
    bloomGui = bloomParameters;
}

void RenderPlane::SetHDRParameters(HdrGUI* hdrGui)
{
    this->hdrGui = hdrGui;
}
