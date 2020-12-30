#include "DemoLogic.h"

void PlayShot()
{
    PlaySound("./resources/soundtrack/shot.wav", NULL, SND_ASYNC);
}

DemoLogic::DemoLogic()
{
    vfx = new ParticleSystem(ParticleSystemType::BILLBOARD_ANIMATED, "./resources/cave/PARTICLE_SYSTEMS/Explosion_5x5.png", 5, 5);
    vfx->isExplosion = true;
    vfx->SetPosition(glm::vec3(3.54f, 0.81f, 23.78f));
}

void DemoLogic::AddBillboards(ParticleSystem* text)
{
    texts.push_back(text);
}

void DemoLogic::AddDeltaTime(float* delta)
{
    deltaTime = delta;
}

void DemoLogic::UpdateDemoLogic()
{
    if (!IS_GAME_RUN)
        return;

    if (ImGui::IsKeyReleased('E') || ImGui::IsKeyReleased('e'))
    {
        if (IS_TALK_1 && !IS_TALK_2)
        {
            UpdateText();
            IS_TALK_2 = true;
        }
        else if (IS_TALK_2 && !IS_TALK_3)
        {
            UpdateText();
            IS_TALK_3 = true;
            artifactLight->SetLinear(10.0f);
            artifactLight->isCastShadow = true;
        }
        else if (IS_TALK_4 && !IS_TALK_5)
        {
            UpdateText();
            modelWeapon->SetDemoType(DEMO::IS_NOTHING);
            IS_TALK_5 = true;
            //PONGO EL ORBE
            modelArtifact->SetDemoType(DEMO::IS_ARTIFACT);
            modelArtifact->transform->model = glm::translate(glm::mat4(1.0f), glm::vec3(10.88f, -1.240f, 32.09f));
            artifactLight->SetPosition(glm::vec3(10.88f, -1.240f, 32.09f));
            artifactLight->SetLinear(10.0f);
            artifactLight->isCastShadow = false;

            weaponLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(15.0f, 1.0f, 1.0f));
            weaponLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(15.0f, 1.0f, 1.0f));

            magicBase->isActive = true;
        }
        else if (IS_WALL_DESTROY && !IS_TALK_STATUE_1)
        {
            UpdateText();
            IS_TALK_STATUE_1 = true;
        }
    }

    if (ImGui::IsKeyReleased('F') || ImGui::IsKeyReleased('f'))
    {
        if (IS_TALK_3 && !IS_TALK_4)
        {
            UpdateText();
            IS_TALK_4 = true;
            //COJO EL ORBE
            modelArtifact->SetDemoType(DEMO::IS_APPEARED);
            artifactLight->isCastShadow = false;
            artifactLight->SetLinear(0.0f);
        }
        else if(IS_TALK_5 && !IS_WEAPON_IN_HAND)
        {
            modelWeapon->SetDemoType(DEMO::IS_APPEARED);
            IS_WEAPON_IN_HAND = true;
            weaponLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(0.0f));
            weaponLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(0.0f));
            magicBase->isActive = false;
        }
    }

    if (ImGui::IsMouseClicked(0))
    {
        if (IS_WEAPON_IN_HAND && !isCurrentActiveShootAnimation && !IS_WALL_DESTROY)
        {
            PlaySound("./resources/soundtrack/shot.wav", NULL, SND_ASYNC);
            activeShootAnimation = true;
            isCurrentActiveShootAnimation = true;
            if (!IS_WALL_DESTROY)
            {
                UpdateText();
                raySystem->ApplyForce();
                IS_WALL_DESTROY = true;
            }
        }
    }

    if (IS_WALL_DESTROY && !playMagicMusic)
    {
        elapsedTimeMusic += *deltaTime;

        if (elapsedTimeMusic > 3.5f)
        {
            PlaySound("./resources/soundtrack/magic.wav", NULL, SND_ASYNC);
            playMagicMusic = true;
        }
    }

    ShootAnimation();
}

void DemoLogic::AddRayPhysicSystem(RayCastSystem* raySys)
{
    raySystem = raySys;
}

void DemoLogic::ShootAnimation()
{
    if (activeShootAnimation)
    {
        elapsedTime += *deltaTime;

        if (elapsedTime > 0.2f)
        {
            elapsedTime = 0.0f;
            activeShootAnimation = false;
            armWeapon->transform->model = glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
            armWeapon->transform->model = glm::translate(armWeapon->transform->model, glm::vec3(0.3f, -0.2f, 0.3f));
            isCurrentActiveShootAnimation = false;
        }
    }
}

glm::vec3 DemoLogic::lerp(glm::vec3 v0, glm::vec3 v1, float t)
{
    return (1 - t) * v0 + t * v1;
}
