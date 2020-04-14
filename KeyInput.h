#pragma once

#include "glm/glm.hpp"
#include "Camera.h"
#include "Model.h"

void processInput(GLFWwindow* window)
{
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
        glfwSetWindowShouldClose(window, true);
}

void processMouseInput(GLFWwindow* window, Camera* cam, std::vector<Model*> sceneModels)
{
    if (ImGui::IsMouseClicked(0))
    {
        glm::vec2 clickPos = glm::vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);

        unsigned int idModelSelected;
        bool isClicked = false;
        float distRay = INFINITE;

        for (unsigned int i = 0; i < sceneModels.size(); i++)
        {
            //Experimental
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            float xVal = ((2.0f * clickPos.x) / (float)width) - 1.0f;
            float yVal = 1.0f - (2.0f * clickPos.y) / (float)height;
            float zVal = -1.0f;

            glm::vec3 ray_nds = glm::vec3(xVal, yVal, zVal);
            glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, 1.0, 1.0);
            glm::vec4 ray_eye = inverse(cam->projection * cam->view * sceneModels.at(i)->model) * ray_clip;
            glm::vec3 ray_wor = glm::normalize(ray_eye);

              
            glm::vec3 ray_orig = inverse(sceneModels.at(i)->model) * glm::vec4(cam->cameraPos, 1.0f);

            float dist = sceneModels.at(i)->checkClickMouse(ray_orig, ray_wor);
            if (dist > 0.0f && dist < distRay)
            {
                isClicked = true;
                distRay = dist;
                idModelSelected = i;
            }
        }

        for (unsigned int i = 0; i < sceneModels.size(); i++)
            sceneModels.at(i)->isSelected(false);

        if (isClicked)
        {
            sceneModels.at(idModelSelected)->isSelected(true);
        }           
    }
}
