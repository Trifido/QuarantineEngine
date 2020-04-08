#pragma once

void processInput(GLFWwindow* window)
{
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
        glfwSetWindowShouldClose(window, true);
}
