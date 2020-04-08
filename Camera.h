#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "libs/glm/glm.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include "libs/glm/gtc/type_ptr.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>

class Camera
{
private: 
   
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const float cameraSpeed = 10.0f;

    bool firstMouse = true;
    float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float fov = 45.0f;

    float WIDTH, HEIGHT;
    
public:
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::mat4 view;
    glm::mat4 projection;
    Camera(float width, float height);
    void CameraController();
    void EditorScroll();
    void EditorRotate();
};

#endif
