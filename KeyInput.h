#pragma once

#include "Camera.h"
#include "Pivot.h"
#include "Model.h"
#include "IntersectionED.h"

class KeyInput
{
public:
    glm::vec2 clickPos;
    Model* modelSelected;
    bool isClicked = false;
    float distRay = FLT_MAX;
    float dragValue = 0.0f;
    UIRay *ray;

    KeyInput() {}

    void processInput(GLFWwindow* window)
    {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
            glfwSetWindowShouldClose(window, true);
    }

    void processMouseInput(GLFWwindow* window, Camera* cam, std::vector<Model*> sceneModels, Pivot* pivote)
    {
        if (ImGui::IsMouseClicked(0))
        {
            modelSelected = nullptr;
            isClicked = false;
            distRay = FLT_MAX;

            clickPos = glm::vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);

            //First CheckPivot
            if (!isClicked || !pivote->isRendered)
            {
                for (unsigned int i = 1; i < sceneModels.size(); i++)
                {
                    CheckRayGeometry(window, cam, sceneModels.at(i));
                    sceneModels.at(i)->isSelected(false);
                }

                if (isClicked)
                {
                    //pivote->AttachModel(modelSelected);
                    pivote->isRendered = true;
                    modelSelected->isSelected(true);
                    pivote->isRendered = pivote->CheckCollision(clickPos, ray);
                }
                else
                {
                    if(pivote->isRendered)
                        pivote->isRendered = pivote->CheckCollision(clickPos, ray);
                }
            }
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            if (pivote->isRendered)
            {
               //pivote->CheckXAxis(clickPos, ray);
                //if (!pivote->CheckXAxis(clickPos, ray))
                //    if(!pivote->CheckYAxis(clickPos, ray))
                //        pivote->CheckZAxis(clickPos, ray);

                //vote->CheckYAxis(clickPos, ray);
                //pivote->CheckZAxis(clickPos, ray);
            }
        }
        else
        {
            pivote->isFirstDragX = true;
            pivote->isFirstDragY = true;
        }
    }

    void CheckRayGeometry(GLFWwindow* window, Camera* cam, Model* model)
    {
        //Experimental
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float xVal = ((2.0f * clickPos.x) / (float)width) - 1.0f;
        float yVal = 1.0f - (2.0f * clickPos.y) / (float)height;
        float zVal = -1.0f;

        glm::vec3 ray_nds = glm::vec3(xVal, yVal, zVal);
        glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, 1.0, 1.0);
        glm::vec4 ray_eye = inverse(cam->projection * cam->view * model->transform->model) * ray_clip;
        glm::vec3 ray_wor = glm::normalize(ray_eye);
        glm::vec3 ray_orig = inverse(model->transform->model) * glm::vec4(cam->cameraPos, 1.0f);

        this->ray = new UIRay(cam->cameraPos, ray_clip, cam->projection * cam->view);
        //this->ray = new UIRay(cam->cameraPos, ray_wor, cam->projection * cam->view);

        float dist = model->checkClickMouse(ray_orig, ray_wor);
        if (dist > 0.0f && dist < distRay)
        {
            modelSelected = model;
            isClicked = true;
            distRay = dist;
        }
    }
};


