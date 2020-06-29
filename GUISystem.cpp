#include "GUISystem.h"

#include "imgui.h"

void GUISystem::DrawGUI()
{
    DrawMainMenuBar();
}

void GUISystem::DrawMainMenuBar()
{
    static bool isActiveForwardRender = true;
    static bool isActiveDeferredRender = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            //ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Render"))
        {
            if (ImGui::MenuItem("Forward Render", NULL, &isActiveForwardRender))
            {
                isActiveForwardRender = true;
                isActiveDeferredRender = false;
            }
            if (ImGui::MenuItem("Deferred Render", NULL, &isActiveDeferredRender))
            {
                isActiveDeferredRender = true;
                isActiveForwardRender = false;
            }
            if (ImGui::BeginMenu("Render Mode"))
            {
                if (ImGui::MenuItem("Solid", NULL)) { draw_mode = DrawMode::DTRIANGLES; }
                if (ImGui::MenuItem("Wireframe", NULL)) { draw_mode = DrawMode::DLINES; }
                if (ImGui::MenuItem("Point", NULL)) { draw_mode = DrawMode::DPOINTS; }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    isForward = isActiveForwardRender;
}

bool GUISystem::isForwardRender()
{
    return isForward;
}
