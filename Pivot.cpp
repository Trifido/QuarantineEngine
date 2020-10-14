#include "Pivot.h"

Pivot::Pivot()
{
    pivotModel = new Model("resources/3DModels/internal/pivote.fbx");
    pivotShader = new Shader("shaders/internalShader.vert", "shaders/internalShader.frag");

    pivotModel->matHandle.EditMaterial(MaterialComponent::SHADER1, pivotShader);
    pivotModel->matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INTERNAL);
    pivotModel->CAST_SHADOW = false;
    pivotModel->isSelectableModel = true;
    isRendered = false;
    pivotModel->colliders.push_back(new Collider(ColliderType::BOX));
 /*   pivotModel->colliders.push_back(new Collider(ColliderType::BOX));*/
    /*pivotModel->colliders.push_back(new Collider(ColliderType::BOX));*/

    //X axis
    pivotModel->colliders.at(0)->transform->AttachTo(pivotModel->transform);
    pivotModel->colliders.at(0)->meshCollider.ScaleMeshCollider(glm::vec3(0.5, 0.1, 0.1));
    pivotModel->colliders.at(0)->transform->model = glm::translate(pivotModel->colliders.at(0)->transform->model, glm::vec3(0.1, -0.55, -0.55));
    //Y axis
    //pivotModel->colliders.at(1)->transform->AttachTo(pivotModel->transform);
    //pivotModel->colliders.at(1)->meshCollider.ScaleMeshCollider(glm::vec3(0.1, 0.5, 0.1));
    //pivotModel->colliders.at(1)->transform->model = glm::translate(pivotModel->colliders.at(1)->transform->model, glm::vec3(-0.55, 0.1, -0.55));
    //////Z axis
    //pivotModel->colliders.at(2)->transform->AttachTo(pivotModel->transform);
    //pivotModel->colliders.at(2)->meshCollider.ScaleMeshCollider(glm::vec3(0.1, 0.1, 0.5));
    //pivotModel->colliders.at(2)->transform->model = glm::translate(pivotModel->colliders.at(2)->transform->model, glm::vec3(-0.55, -0.55, 0.1));
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
    pivotModel->SetModelHierarchy();
    if (isRendered)
    {
        pivotModel->Draw();
                                                                                                            //Esto es temporal
        for (int i = 0; i < pivotModel->colliders.size(); i++)
            pivotModel->colliders.at(i)->DrawCollider();
    }
}

bool Pivot::CheckXAxis(glm::vec2 clickPosition, UIRay *ray)
{
    if (isFirstDragX)
    {
        if (pivotModel->colliders.at(0)->IsRayCollision(ray))
        {
            lastX = clickPosition.x;
            activeDragX = true;
            activeDragY = false;
            //activeDragZ = false;
            isFirstDragX = false;
        }
        else
        {
            activeDragX = false;
        }
    }

    if (!isFirstDragX && activeDragX)
    {
        //activeDragY = false;
        float xoffset = ImGui::GetIO().MousePos.x - lastX;
        lastX = ImGui::GetIO().MousePos.x;

        float sensitivity = 0.005f; // change this value to your liking
        xoffset *= sensitivity;

        pivotModel->transform->position.x += xoffset;
        pivotModel->TranslationTo(pivotModel->transform->position);
    }

    return activeDragX;
}

bool Pivot::CheckYAxis(glm::vec2 clickPosition, UIRay* ray)
{
    if (isFirstDragY)
    {
        if (pivotModel->colliders.at(1)->IsRayCollision(ray))
        {
            lastY = clickPosition.y;
            //activeDragZ = false;
            activeDragY = true;
            activeDragX = false;
            isFirstDragY = false;
        }
        else
        {
            activeDragY = false;
        }
    }

    if(!isFirstDragY && activeDragY)
    {
        float yoffset = lastY - ImGui::GetIO().MousePos.y; // reversed since y-coordinates go from bottom to top
        lastY = ImGui::GetIO().MousePos.y;

        float sensitivity = 0.005f; // change this value to your liking
        yoffset *= sensitivity;

        pivotModel->transform->position.y += yoffset;
        pivotModel->TranslationTo(pivotModel->transform->position);
    }

    return activeDragY;
}

bool Pivot::CheckZAxis(glm::vec2 clickPosition, UIRay* ray)
{
    if (pivotModel->colliders.at(2)->IsRayCollision(ray))
    {
        lastX = clickPosition.x;
        activeDragZ = true;
        activeDragX = false;
        activeDragY = false;
    }

    if(activeDragZ)
    {
        float xoffset = lastX - ImGui::GetIO().MousePos.x; // reversed since y-coordinates go from bottom to top
        lastX = ImGui::GetIO().MousePos.x;

        float sensitivity = 0.005f;
        xoffset *= sensitivity;

        pivotModel->transform->position.z += xoffset;
        pivotModel->TranslationTo(pivotModel->transform->position);
    }

    return activeDragZ;
}

bool Pivot::CheckCollision(glm::vec2 clickPosition, UIRay* ray)
{
    for (int i = 0; i < pivotModel->colliders.size(); i++)
    {
        if (pivotModel->colliders.at(i)->IsRayCollision(ray))
        {
            return true;
        }
    }
    return false;
}
