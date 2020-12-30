#pragma once

#ifndef RAYCAST_SYSTEM
#define RAYCAST_SYSTEM

#include "PhysicSystem.h"
#include "Camera.h"

class RayCastSystem
{
private:
    rp3d::Vector3 startPosition;
    rp3d::Vector3 endPosition;
    PhysicSystem* pSystem;
    Camera* cam;
public:
    RayCastSystem() {}
    RayCastSystem(PhysicSystem* pSystem, Camera* cam);
    bool isHit = false;
    reactphysics3d::RaycastInfo info;
    bool LaunchRay();
    void ApplyForce();
};
#endif // !RAYCAST_SYSTEM
