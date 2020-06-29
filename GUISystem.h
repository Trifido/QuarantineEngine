#pragma once

#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include "Material.h"

static class GUISystem
{
private:
    bool isForward = true;
    DrawMode draw_mode = DrawMode::DTRIANGLES;
public:
    GUISystem() {};
    void DrawGUI();
    void DrawMainMenuBar();
    bool isForwardRender();
    DrawMode GetDrawMode() { return draw_mode; }
};

#endif
