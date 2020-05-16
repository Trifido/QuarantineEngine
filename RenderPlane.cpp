#include "RenderPlane.h"

RenderPlane::RenderPlane()
{
    gammaValue = 1.0f;
    exposureValue = 1.0f;
    pingpongIdPass = true;
    screenRenderShader = new Shader("shaders/renderPass.vert", "shaders/renderPass.frag");
    shaderBlur = new Shader("shaders/renderPass.vert", "shaders/blurPass.frag");
    shaderBloomFinal = new Shader("shaders/renderPass.vert", "shaders/finalBloom.frag"); 
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
    screenRenderShader->setInt("screenTexture", 0);
    screenRenderShader->setFloat("gamma", gammaValue);
    screenRenderShader->setFloat("exposure", exposureValue);

    shaderBlur->use();
    shaderBlur->setInt("image", 0);

    shaderBloomFinal->use(); 
    shaderBloomFinal->setInt("bloom", bloomValue);
    shaderBloomFinal->setFloat("exposure", exposureValue); 
    shaderBloomFinal->setInt("scene", 0);
    shaderBloomFinal->setInt("bloomBlur", 1);
}

void RenderPlane::FinalRenderPass()
{
    screenRenderShader->use();

    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetFinalRender());	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::FinalRenderBloom()
{
    shaderBloomFinal->use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetMRTRender());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboSystem->GetPingPongRender(!pingpongIdPass));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderPlane::RenderBlur()
{
    bool first_iteration = true;
    unsigned int amount = 10;
    shaderBlur->use(); 
    for (unsigned int i = 0; i < amount; i++)
    {
        fboSystem->PingPongPass(pingpongIdPass);
        shaderBlur->setInt("horizontal", pingpongIdPass);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? fboSystem->GetMRTRender(1) : fboSystem->GetPingPongRender(!pingpongIdPass));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        pingpongIdPass = !pingpongIdPass;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
