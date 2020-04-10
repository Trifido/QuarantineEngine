#pragma once

#ifndef SHADER_H
#define SHADER_H

//#include <GL/gl3w.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdlib.h>

#include "Texture.h"
#include "Light.h"
#include "Camera.h"
#include "glm/glm.hpp"


class Shader
{
public:
    // the program ID
    unsigned int ID;
    Camera* mainCamera;
    std::vector<Light*> lights;
    std::vector<Texture*> textures;
    std::vector<std::string> nameTexture;
    unsigned int numTextures = 0;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec3(const std::string& name, glm::vec3& pos) const;
    void setMat4(const std::string& name, glm::mat4& mat) const;
    void AddTexture(Texture* tex, const std::string& name = NULL);
    void ActivateTextures();
    void AddLight(Light* light);
    void AddLight(std::vector<Light*> lightVec);
    void ActivateLights();
    void AddCamera(Camera* cam);
    void ActivateCamera();
};

#endif

