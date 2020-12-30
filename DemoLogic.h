#pragma once
#ifndef DEMO_LOGIC_H
#define DEMO_LOGIC_H

#include "Model.h"
#include "ParticleSystem.h"
#include "RayCastSystem.h"
#include "GodRay.h"

#include <thread>
#include <Mmsystem.h>
#include <mciapi.h>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")

class DemoLogic
{
public:
    RayCastSystem* raySystem;
    GodRay* magicBase = nullptr;
    float weaponRotation = 0.0f;
    Model* modelWeapon = nullptr;
    Model* armWeapon = nullptr;
    Model* modelArtifact = nullptr;
    Model* modelStatue = nullptr;
    Light* artifactLight = nullptr;
    Light* weaponLight = nullptr;
    bool IS_GAME_RUN = true;
    bool IS_WEAPON_SHOWED = false;
    float* deltaTime;

    //STATES BOOL
    bool IS_TALK_1 = true;
    bool IS_TALK_2 = false;
    bool IS_TALK_3 = false;
    bool IS_TALK_4 = false;
    bool IS_TALK_5 = false;
    bool IS_ARTIFACT_IN_HAND = false;
    bool IS_WEAPON_IN_HAND = false;
    bool IS_WALL_DESTROY = false;
    bool IS_TALK_STATUE_1 = false;
    bool activeShootAnimation = false;
    bool isCurrentActiveShootAnimation = false;
    bool playMagicMusic = false;
    unsigned int NUM_TEXTS = 7;

    std::vector<ParticleSystem*> texts;
    ParticleSystem* vfx;
    unsigned int currentText = 0;
    float elapsedTime = 0.0f;
    float elapsedTimeMusic = 0.0f;

    DemoLogic();
    void AddBillboards(ParticleSystem* text);
    ParticleSystem inline* GetCurrentText() { return texts.at(currentText); }
    void inline UpdateText() { if(currentText < NUM_TEXTS) currentText++; }
    void AddDeltaTime(float* delta);
    void UpdateDemoLogic();
    void inline AddArtifactLight(Light* lArt) { artifactLight = lArt; }
    void inline AddWeaponLight(Light* lWe) { weaponLight = lWe; }
    void AddRayPhysicSystem(RayCastSystem* raySys);
    void ShootAnimation();
    glm::vec3 lerp(glm::vec3 v0, glm::vec3 v1, float t);
    void inline AddVFXExplosion(ParticleSystem* ps) { vfx = ps; }
};

#endif // !DEMO_LOGIC_H
