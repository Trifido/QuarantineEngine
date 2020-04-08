#pragma once

#ifndef MODEL_H
#define MODEL_H

#include "libs/glm/glm.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include "libs/glm/gtc/type_ptr.hpp"

#include <GLFW/glfw3.h>

class Model
{    
public:
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;

    Model();

    void Rotation(float radians, glm::vec3 axis);
    void TranslationTo(glm::vec3 position);
    void ScaleTo(glm::vec3 scale);
    void ResetModel();
};

#endif
