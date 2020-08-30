#include "Pivot.h"

Pivot::Pivot()
{
    pivotModel = new Model("resources/3DModels/internal/pivote.fbx");
    pivotShader = new Shader("shaders/internalShader.vert", "shaders/internalShader.frag");

    pivotModel->matHandle.EditMaterial(MaterialComponent::SHADER1, pivotShader);
    pivotModel->matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);
    pivotModel->CAST_SHADOW = false;
    pivotModel->isSelectableModel = true;
    pivotModel->TranslationTo(glm::vec3(3.0, 0.0, 0.0));
    isRendered = false;
    pivotModel->colliders.push_back(new Collider(ColliderType::BOX));
}

void Pivot::AttachModel(Model* model)
{
    isRendered = true;
    pivotModel->transform->position = model->transform->position;
    pivotModel->TranslationTo(pivotModel->transform->position);
    model->AttachModel(pivotModel);
}

void Pivot::DrawPivot()
{
    if (isRendered)
    {
        pivotModel->Draw();
                                                                                                            //Esto es temporal
        for (int i = 0; i < pivotModel->colliders.size(); i++)
            pivotModel->colliders.at(i)->DrawCollider();
    }
}

void Pivot::CheckXAxis(glm::vec2 clickPosition)
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        if (firstMouse)
        {
            lastX = clickPosition.x;
            firstMouse = false;
        }
        else
        {
            float xoffset = ImGui::GetIO().MousePos.x - lastX;
            lastX = ImGui::GetIO().MousePos.x;

            float sensitivity = 0.01f; // change this value to your liking
            xoffset *= sensitivity;

            pivotModel->transform->position.x += xoffset;
            pivotModel->TranslationTo(pivotModel->transform->position);
            std::cout << pivotModel->transform->position.x << " " << pivotModel->transform->position.y << std::endl;
        }
    }
    else
    {
        firstMouse = true;
    }
}

void Pivot::CheckYAxis(glm::vec2 clickPosition)
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        if (firstMouse)
        {
            lastY = clickPosition.y;
            firstMouse = false;
        }
        else
        {
            float yoffset = lastY - ImGui::GetIO().MousePos.y; // reversed since y-coordinates go from bottom to top
            lastY = ImGui::GetIO().MousePos.y;

            float sensitivity = 0.01f; // change this value to your liking
            yoffset *= sensitivity;

            pivotModel->transform->position.y += yoffset;
            pivotModel->TranslationTo(pivotModel->transform->position);
        }
    }
    else
    {
        firstMouse = true;
    }
}
