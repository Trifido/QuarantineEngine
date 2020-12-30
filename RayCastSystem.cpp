#include "RayCastSystem.h"

RayCastSystem::RayCastSystem(PhysicSystem* pSystem, Camera* cam)
{
    this->cam = cam;
    this->pSystem = pSystem;
}

bool RayCastSystem::LaunchRay()
{
    startPosition = rp3d::Vector3(cam->cameraPos.x, cam->cameraPos.y, cam->cameraPos.z);
    glm::vec3 endPos = (cam->cameraFront + cam->cameraPos) * 10.0f;
    endPosition = rp3d::Vector3(endPos.x, endPos.y, endPos.z);
    rp3d::Ray ray(startPosition, endPosition);

    for (unsigned int i = 0; i < pSystem->rigidColliders.size(); i++)
    {
        isHit = pSystem->rigidColliders.at(i)->raycast(ray, info);
    }

    printf("Start Point: %f, %f, %f\n", startPosition.x, startPosition.y, startPosition.z);
    printf("End Point: %f, %f, %f\n", endPosition.x, endPosition.y, endPosition.z);

    if (isHit)
        printf("Golpeado\n");
    else
        printf("NOPE\n");

    return isHit;
}

void RayCastSystem::ApplyForce()
{
    for (unsigned int i = 0; i < pSystem->rBodies.size(); i++)
    {
        if (pSystem->rBodies.at(i).second->getType() == rp3d::BodyType::DYNAMIC)
        {
            float forceZ = 1000 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (8000 - 1000)));
            float forceX = ((float)rand()) / RAND_MAX * 8000.0 - 1000.0;
            pSystem->rBodies.at(i).second->applyForceToCenterOfMass(rp3d::Vector3(forceX, 0.0, -forceZ));
        }
    }
}
