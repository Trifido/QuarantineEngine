#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include "HeadersRequeriments.h"
#include "MaterialHandle.h"
#include "Camera.h"
#include "FBOSystem.h"

class Skybox
{
private:
    unsigned int VAO, VBO;
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    glm::mat3 view = glm::mat3(1.0f);
    glm::mat4 projection;

public:
    //std::vector<std::string> faces
    //{
    //    "resources/skybox/right.jpg",
    //    "resources/skybox/left.jpg",
    //    "resources/skybox/top.jpg",
    //    "resources/skybox/bottom.jpg",
    //    "resources/skybox/front.jpg",
    //    "resources/skybox/back.jpg"
    //};
    Texture* hdrTexture;
    Shader* finalHDR;
    Shader* irradianceShader;
    Shader* prefilterShader;
    Shader* brdfShader;

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[6] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    std::vector<std::string> faces
    { 
        "resources/space/skybox/Nebula_light_right.png", 
        "resources/space/skybox/Nebula_light_left.png",
        "resources/space/skybox/Nebula_light_up.png", 
        "resources/space/skybox/Nebula_light_down.png",
        "resources/space/skybox/Nebula_light_back.png",
        "resources/space/skybox/Nebula_light_front.png"
    };

    MaterialHandle* matHandle;
    Skybox();
    Skybox(bool isSkybox);
    void PreRender();
    void Render();
    void RenderHDR(Camera* cam, unsigned int idRender);
    void RenderIrradianceMap(Camera* cam, unsigned int idRender, unsigned int idRenderDest);
    void SetUp();
    void AddCamera(Camera* cam);
    void SetUpCubeMap();
    void SetUpPrefilterMap(unsigned int idRender, FBO* fbo);
    void SetHdrTexture(Texture* textaux) { hdrTexture = textaux; }
    void DrawCube();
    void DrawQuad();
};

#endif
