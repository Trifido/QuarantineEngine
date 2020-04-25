#pragma once

#ifndef SHADER_H
#define SHADER_H

//#include <GL/gl3w.h> // include glad to get all the required OpenGL headers

#include "HeadersRequeriments.h"


#include "Texture.h"
#include "Light.h"
#include "Camera.h"

//#include <GL/gl3w.h>

void CheckVertexCompilation(unsigned int vertexShader);
void CheckFragmentCompilation(unsigned int fragShader);
void CheckGeometryCompilation(unsigned int geoShader);
void CheckProgramLink(unsigned int shaderProgram);

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
    Shader(const char* vertexPath, const char* geomtryPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec3(const std::string& name, glm::vec3& pos) const;
    void setMat3(const std::string& name, glm::mat3& mat) const;
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

