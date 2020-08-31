#pragma once

#ifndef INTERSECTION_ED_H
#define INTERSECTION_ED_H

#include "HeadersRequeriments.h"

struct UIRay
{
    glm::vec3 ray_dir;
    glm::vec3 ray_orig;
    glm::mat4 PV;

    UIRay() {};
    UIRay(glm::vec3 ray_orig, glm::vec3 ray_wor, glm::mat4 PV)
    {
        this->ray_dir = ray_wor;
        this->ray_orig = ray_orig;
        this->PV = PV;
    };
};

#endif
