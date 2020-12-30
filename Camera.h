#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "HeadersRequeriments.h"
#include "Transform.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

class Camera
{
private: 
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const float cameraSpeed = 10.0f;

    bool firstMouse = true;
    float lastX = 1280.0f / 2.0;
    float lastY = 720.0 / 2.0;
    float fov = 45.0f;
    float nearPlane, farPlane;
    float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
public:
    float WIDTH, HEIGHT;
    //glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraFront = glm::vec3(0.765475571f, -0.0854168385f, 0.637770593f);
    //glm::vec3 cameraFront = glm::vec3(0.0f, -0.5f, -1.0f);
    glm::vec3 cameraPos = glm::vec3(-32.4065971f, 0.972609758f, 25.2997189f);
    //glm::vec3 cameraPos = glm::vec3(0.0f, 15.0f, 15.0f);
    glm::mat4 view;
    glm::mat4 projection;
    CustomTransform* transform;
    glm::mat4 MVP;
    glm::mat4 VP;
    Camera(float width, float height);
    void CameraController(float deltaTime);
    void EditorScroll();
    void EditorRotate();
    void AttachTo(CustomTransform* model);
    float GetFOV() { return fov; }
    float* GetRawFOV() { return &fov; }
    float* GetRawNearPlane() { return &nearPlane; }
    float* GetRawFarPlane() { return &farPlane; }
    float* GetRawCameraPosition() { return &cameraPos[0]; }
    float* GetRawCameraFront() { return &cameraFront[0]; }
    void CheckCameraAttributes(float* positionCamera, float* frontCamera, float fov, float nearPlane, float farPlane);
    void InvertPitch(float heightPos);
};

#endif
