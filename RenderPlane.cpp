#include "RenderPlane.h"

RenderPlane::RenderPlane()
{
    screenRenderShader = new Shader("shaders/renderPass.vert", "shaders/renderPass.frag");
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
}

void RenderPlane::DrawFrom(unsigned int & textureColorbuffer)
{
    screenRenderShader->use();
    //screenRenderShader->setFloat("near_plane", 1.0f);
    //screenRenderShader->setFloat("far_plane", 7.5f);

    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
