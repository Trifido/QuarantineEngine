#pragma once

#ifndef CONTENTSSBO_H
#define CONTENTSSBO_H

#include "HeadersRequeriments.h"

struct ssbo_PointLight_t
{
    glm::vec3 diffuse; float __paddingn1;
    glm::vec3 position; float __paddingn2;
    float constant; float __padding1[3];
    float linear; float __padding2[3];
    float quadratic; float __padding3[3];
    float far_plane; float __padding4[3];
};

struct ssbo_data_t
{
    ssbo_PointLight_t pointLights[2];
};

#endif
