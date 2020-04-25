#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include "HeadersRequeriments.h"
#include "MaterialHandle.h"
#include "Camera.h"


class Skybox
{
private:
    unsigned int VAO, VBO;
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

    std::vector<std::string> faces
    {
        "resources/Starscape.png",
        "resources/Starscape.png",
        "resources/Starscape.png",
        "resources/Starscape.png",
        "resources/Starscape.png",
        "resources/Starscape.png"
    };

    MaterialHandle* matHandle;
    Skybox();

    void Render();
    void SetUp();
    void AddCamera(Camera* cam);
};

#endif
