#include "RenderSystem.h"

RenderSystem::RenderSystem(GLFWwindow* window, ImVec4* clearColor)
{
    deltaTime = lastFrame = 0.0f;
    this->window = window;
    clear_color = clearColor;
    num_dir_cast_shadow = 0;
    num_omni_cast_shadow = 0;
    num_spot_cast_shadow = 0;
    precookSkybox = nullptr;
    ssboSystem = nullptr;
    guiSystem = new GUISystem();

    //Load Pivot Model
    pivot = new Pivot();
    this->AddModel(pivot->GetModel());
}

void RenderSystem::ComputeDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void RenderSystem::Clean()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    for (unsigned int i = 0; i < models.size(); i++)
    {
        models.at(i)->DeleteGPUInfo();
        //delete models.at(i);
    }

    for(unsigned int i = 0; i < fresnelModels.size(); i++)
    {
        fresnelModels.at(i)->DeleteGPUInfo();
    }

    for(unsigned int i = 0; i < particleSystems.size(); i++)
    {
        particleSystems.at(i)->DeleteGPUInfo();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void RenderSystem::AddGlfWindow(GLFWwindow* window)
{
    this->window = window;
}

void RenderSystem::AddLight(Light* lights)
{
    this->lights.push_back(lights);

    if (lights->GetType() == TypeLight::DIRL)
    {
        if (num_dir_cast_shadow < LIMIT_DIR_CAST_SHADOW)
        {
            this->shadowCastDirLights.push_back(lights);
            this->shadowCastGeneralLights.push_back(lights);
            lights->EditLightComponent(LightComponent::LIGHT_CAST_SHADOW, true);
            num_dir_cast_shadow++;
        }
    }
    else if (lights->GetType() == TypeLight::POINTLIGHT)
    {
        if (num_omni_cast_shadow < LIMIT_OMNI_CAST_SHADOW)
        {
            this->shadowCastOmniLights.push_back(lights);
            this->shadowCastGeneralLights.push_back(lights);
            lights->EditLightComponent(LightComponent::LIGHT_CAST_SHADOW, true);
            num_omni_cast_shadow++;
        }
    }
    else if (lights->GetType() == TypeLight::SPOTL)
    {
        if (num_spot_cast_shadow < LIMIT_SPOT_CAST_SHADOW)
        {
            this->shadowCastSpotLights.push_back(lights);
            this->shadowCastGeneralLights.push_back(lights);
            lights->EditLightComponent(LightComponent::LIGHT_CAST_SHADOW, true);
            num_spot_cast_shadow++;
        }
    }
    
}

void RenderSystem::AddLight(BoundingLight* bdLight)
{
    this->lights.push_back(bdLight->light);
    if (num_omni_cast_shadow < LIMIT_OMNI_CAST_SHADOW)
    {
        this->shadowCastOmniLights.push_back(bdLight->light);
        this->shadowCastGeneralLights.push_back(bdLight->light);
        bdLight->light->EditLightComponent(LightComponent::LIGHT_CAST_SHADOW, true);
        num_omni_cast_shadow++;
    }
    this->boundingModels.push_back(bdLight);
}

void RenderSystem::AddCamera(Camera* cam)
{
    this->cameras.push_back(cam);
}

void RenderSystem::AddModel(Model* model)
{
    this->models.push_back(model);
}

void RenderSystem::AddFresnelModel(Water* fresnelModel)
{
    this->fresnelModels.push_back(fresnelModel);
}

void RenderSystem::AddParticleSystem(ParticleSystem* system)
{
    this->particleSystems.push_back(system);
}

void RenderSystem::AddRenderVolume(RenderVolume* volume)
{
    this->renderVolumes.push_back(volume);
}

void RenderSystem::AddPreCookSkybox(Skybox* skyHDR)
{
    precookSkybox = skyHDR;
}

void RenderSystem::RenderSkyBox()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    if (precookSkybox == nullptr)     
    {
        skybox->Render();
    }
    else
    {
        precookSkybox->RenderHDR(cameras[0], fboSystem->GetSkyboxRender(0));
    }
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}

void RenderSystem::RenderDirectionalShadowMap()
{
    glCullFace(GL_FRONT);
    for (int idLight = 0; idLight < this->shadowCastDirLights.size(); idLight++)
    {
        fboSystem->DirShadowPass(idLight);
        glViewport(0, 0, 4048, 4048);
        glClear(GL_DEPTH_BUFFER_BIT);
               
        for (int idModel = 0; idModel < models.size(); idModel++)
            models[idModel]->DrawShadow(shadowCastDirLights.at(idLight)->lightSpaceMatrix);
    }
    int idLightGeneral = this->shadowCastDirLights.size();
    for (int idLight = idLightGeneral; idLight < this->shadowCastSpotLights.size(); idLight++)
    {
        fboSystem->DirShadowPass(idLight);
        glViewport(0, 0, 4048, 4048);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (int idModel = 0; idModel < models.size(); idModel++)
            models[idModel]->DrawShadow(shadowCastSpotLights.at(idLight)->lightSpaceMatrix);
    }
    glCullFace(GL_BACK);
}

void RenderSystem::RenderOmnidirectionalShadowMap()
{
    glCullFace(GL_FRONT);
    for (int idLight = 0; idLight < this->shadowCastOmniLights.size(); idLight++)
    {
        fboSystem->OmniShadowPass(idLight);
        glViewport(0, 0, 4048, 4048);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (int idModel = 0; idModel < models.size(); idModel++)
        {
            if(models[idModel]->matHandle.type != MaterialType::EMISSIVE_LIT &&
                models[idModel]->matHandle.type != MaterialType::BOUNDING_VOLUME)
                models[idModel]->DrawShadow(shadowCastOmniLights.at(idLight)->lightSpaceMatrices, shadowCastOmniLights.at(idLight)->GetPosition(), shadowCastOmniLights.at(idLight)->GetFarplane());
        }
    }
    glCullFace(GL_BACK);
}

void RenderSystem::RenderVolumeShadow()
{
    glDisable(GL_CULL_FACE);
    for (int i = 0; i < solidModels.size(); i++)
    {
        //Comprobamos la jerarqu�a de los modelos 3D
        solidModels[i]->SetModelHierarchy();

        if (solidModels[i]->CAST_SHADOW)
        {
            if (solidModels[i]->matHandle.type != MaterialType::EMISSIVE_LIT && solidModels[i]->matHandle.type != MaterialType::BOUNDING_VOLUME)
            {
                glm::vec3 camViewLight = cameras[0]->view * glm::vec4(shadowCastOmniLights.at(0)->GetPosition(), 1.0);
                solidModels[i]->DrawVolumeShadow(camViewLight);
            }
        }
    }

    for (int i = 0; i < outLineModels.size(); i++)
    {
        outLineModels[i]->SetModelHierarchy();

        if (outLineModels[i]->CAST_SHADOW)
        {
            if (outLineModels[i]->matHandle.type != MaterialType::EMISSIVE_LIT && outLineModels[i]->matHandle.type != MaterialType::BOUNDING_VOLUME)
            {
                glm::vec3 camViewLight = cameras[0]->view * glm::vec4(shadowCastOmniLights.at(0)->GetPosition(), 1.0);
                outLineModels[i]->DrawVolumeShadow(camViewLight);
            }
        }
    }

    glEnable(GL_CULL_FACE);
}

void RenderSystem::RenderDepthMap()
{
    //glDisable(GL_CULL_FACE);
    for (int i = 0; i < solidModels.size(); i++)
    {
        solidModels[i]->SetModelHierarchy();
        solidModels[i]->DrawDepthMap(*cameras[0]->GetRawNearPlane(), *cameras[0]->GetRawFarPlane());
    }

    for (int i = 0; i < outLineModels.size(); i++)
    {
        outLineModels[i]->SetModelHierarchy();
        outLineModels[i]->DrawDepthMap(*cameras[0]->GetRawNearPlane(), *cameras[0]->GetRawFarPlane());
    }
    //glEnable(GL_CULL_FACE);
}

void RenderSystem::RenderSolidModels()
{
    glDisable(GL_CULL_FACE);
    for (int i = 0; i < solidModels.size(); i++)
    {
        //Comprobamos la jerarqu�a de los modelos 3D
        solidModels[i]->SetModelHierarchy();

        //A�adimos el mapa de irradiancia
        solidModels[i]->matHandle.ActivateIrradianceMap(fboSystem->GetSkyboxRender(1), fboSystem->GetPrefilterRender(), fboSystem->GetSkyboxRender(2));

        if (solidModels[i]->CAST_SHADOW)
        {
            for (int idLightD = 0; idLightD < LIMIT_DIR_CAST_SHADOW; idLightD++)
            {
                if (idLightD < this->shadowCastDirLights.size())
                {
                    solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLightD), idLightD, TypeLight::DIRL);
                }
                else
                {
                    solidModels[i]->matHandle.ActivateShadowMap(NULL, idLightD, TypeLight::DIRL);
                }
            }

            for (int idLightS = 0; idLightS < LIMIT_SPOT_CAST_SHADOW; idLightS++)
            {
                if (idLightS < this->shadowCastSpotLights.size())
                {
                    solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLightS), idLightS, TypeLight::SPOTL);
                }
                else
                {
                    solidModels[i]->matHandle.ActivateShadowMap(NULL, idLightS, TypeLight::SPOTL);
                }
            }

            for (int idLightP = 0; idLightP < LIMIT_OMNI_CAST_SHADOW; idLightP++)
            {
                if (idLightP < this->shadowCastOmniLights.size())
                {
                    solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetOmniRender(idLightP), idLightP, TypeLight::POINTLIGHT);
                }
                else
                {
                    solidModels[i]->matHandle.ActivateShadowMap(NULL, idLightP, TypeLight::POINTLIGHT);
                }
            }

            solidModels[i]->matHandle.shader->setBool("isClipPlane", false);
            solidModels[i]->DrawCastShadow(shadowCastGeneralLights);
        }
        else
        {
            solidModels[i]->Draw();
        }
    }
    glEnable(GL_CULL_FACE);
}

void RenderSystem::RenderFPSModels()
{
    glDisable(GL_CULL_FACE);
    for (int i = 0; i < fpsModels.size(); i++)
    {
        ////Comprobamos la jerarqu�a de los modelos 3D
        fpsModels[i]->SetModelHierarchy();

        //A�adimos el mapa de irradiancia
        fpsModels[i]->matHandle.ActivateIrradianceMap(fboSystem->GetSkyboxRender(1), fboSystem->GetPrefilterRender(), fboSystem->GetSkyboxRender(2));

        if (fpsModels[i]->CAST_SHADOW)
        {
            for (int idLight = 0; idLight < this->shadowCastDirLights.size(); idLight++)
                fpsModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLight), idLight, TypeLight::DIRL);

            for (int idLight = 0; idLight < this->shadowCastSpotLights.size(); idLight++)
                fpsModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLight), idLight, TypeLight::SPOTL);

            for (int idLight = 0; idLight < this->shadowCastOmniLights.size(); idLight++)
                fpsModels[i]->matHandle.ActivateShadowMap(fboSystem->GetOmniRender(idLight), idLight, TypeLight::POINTLIGHT);

            fpsModels[i]->matHandle.shader->setBool("isClipPlane", false);
            fpsModels[i]->DrawCastShadow(shadowCastGeneralLights);
        }
        else
        {
            fpsModels[i]->Draw();
        }
    }
    glEnable(GL_CULL_FACE);
}

void RenderSystem::RenderTransparentModels()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < transparentModels.size(); i++)
    {
        transparentModels[i]->Draw();
    }

    glDisable(GL_BLEND);
}

void RenderSystem::RenderInternalModels()
{
    glDisable(GL_DEPTH_TEST);
    pivot->GetModel()->SetModelHierarchy();
    pivot->DrawPivot();

    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::RenderVolumes()
{
    glEnable(GL_CULL_FACE);
    
    for (unsigned int i = 0; i < renderVolumes.size(); i++)
    {
        renderVolumes.at(i)->DrawRenderVolume();
    }

    glDisable(GL_CULL_FACE);
}

void RenderSystem::RenderOcclusionLightScattering()
{
    //glDisable(GL_CULL_FACE);
    for (int i = 0; i < solidModels.size(); i++)
    {
        solidModels[i]->SetModelHierarchy();
        solidModels[i]->DrawOcclusion(shadowCastGeneralLights);
    }
    //for (int i = 0; i < outLineModels.size(); i++)
    //{
    //    outLineModels[i]->SetModelHierarchy();
    //    outLineModels[i]->DrawOcclusion(shadowCastGeneralLights);
    //}
    //glEnable(GL_CULL_FACE);
}

void RenderSystem::ProcessBoundingModels()
{
    for (int i = 0; i < boundingModels.size(); i++)
    {
        boundingModels.at(i)->Draw();
    }
}

void RenderSystem::RenderOutLineModels()
{
    for (int i = 0; i < outLineModels.size(); i++)
    {
        //Comprobamos la jerarqu�a de los modelos 3D
        outLineModels[i]->SetModelHierarchy();

        //A�adimos el mapa de irradiancia
        outLineModels[i]->matHandle.ActivateIrradianceMap(fboSystem->GetSkyboxRender(1), fboSystem->GetPrefilterRender(), fboSystem->GetSkyboxRender(2));

        if (outLineModels[i]->CAST_SHADOW /*&& guiSystem->GetShadowMode() == ShadowType::SHADOW_MAP*/)
        {
            for (int idLightD = 0; idLightD < LIMIT_DIR_CAST_SHADOW; idLightD++)
            {
                if (idLightD < this->shadowCastDirLights.size())
                {
                    outLineModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLightD), idLightD, TypeLight::DIRL);
                }
                else
                {
                    outLineModels[i]->matHandle.ActivateShadowMap(NULL, idLightD, TypeLight::DIRL);
                }
            }

            for (int idLightS = 0; idLightS < LIMIT_SPOT_CAST_SHADOW; idLightS++)
            {
                if (idLightS < this->shadowCastSpotLights.size())
                {
                    outLineModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLightS), idLightS, TypeLight::SPOTL);
                }
                else
                {
                    outLineModels[i]->matHandle.ActivateShadowMap(NULL, idLightS, TypeLight::SPOTL);
                }
            }

            for (int idLightP = 0; idLightP < LIMIT_OMNI_CAST_SHADOW; idLightP++)
            {
                if (idLightP < this->shadowCastOmniLights.size())
                {
                    outLineModels[i]->matHandle.ActivateShadowMap(fboSystem->GetOmniRender(idLightP), idLightP, TypeLight::POINTLIGHT);
                }
                else
                {
                    outLineModels[i]->matHandle.ActivateShadowMap(NULL, idLightP, TypeLight::POINTLIGHT);
                }
            }
            outLineModels[i]->matHandle.shader->setBool("isClipPlane", false);
            outLineModels[i]->DrawCastShadow(shadowCastGeneralLights, true);
        }
        else
        {
            outLineModels[i]->Draw();
        }
    }
}

void RenderSystem::RenderClipPlane(glm::vec4 plane)
{
    glEnable(GL_CULL_FACE);
    for (int i = 0; i < solidModels.size(); i++)
    {
        //Comprobamos la jerarqu�a de los modelos 3D
        solidModels[i]->SetModelHierarchy();

        //A�adimos el mapa de irradiancia
        solidModels[i]->matHandle.ActivateIrradianceMap(fboSystem->GetSkyboxRender(1), fboSystem->GetPrefilterRender(), fboSystem->GetSkyboxRender(2));

        solidModels[i]->matHandle.shader->setBool("isClipPlane", false);
        solidModels[i]->matHandle.shader->setVec4("clip_plane", plane);

        if (solidModels[i]->CAST_SHADOW)
        {
            for (int idLightD = 0; idLightD < LIMIT_DIR_CAST_SHADOW; idLightD++)
            {
                if (idLightD < this->shadowCastDirLights.size())
                {
                    solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLightD), idLightD, TypeLight::DIRL);
                }
                else
                {
                    solidModels[i]->matHandle.ActivateShadowMap(NULL, idLightD, TypeLight::DIRL);
                }
            }

            for (int idLightS = 0; idLightS < LIMIT_SPOT_CAST_SHADOW; idLightS++)
            {
                if (idLightS < this->shadowCastSpotLights.size())
                {
                    solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLightS), idLightS, TypeLight::SPOTL);
                }
                else
                {
                    solidModels[i]->matHandle.ActivateShadowMap(NULL, idLightS, TypeLight::SPOTL);
                }
            }

            for (int idLightP = 0; idLightP < LIMIT_OMNI_CAST_SHADOW; idLightP++)
            {
                if (idLightP < this->shadowCastOmniLights.size())
                {
                    solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetOmniRender(idLightP), idLightP, TypeLight::POINTLIGHT);
                }
                else
                {
                    solidModels[i]->matHandle.ActivateShadowMap(NULL, idLightP, TypeLight::POINTLIGHT);
                }
            }

            solidModels[i]->DrawCastShadow(shadowCastGeneralLights);
        }
        else
        {
            solidModels[i]->Draw();
        }
    }
    glDisable(GL_CULL_FACE);
}

void RenderSystem::RenderFresnelModels()
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (unsigned int i = 0; i < fresnelModels.size(); i++)
    {
        ////Comprobamos la jerarqu�a de los modelos 3D
        fresnelModels[i]->SetModelHierarchy();

        //A�adimos el mapa de irradiancia
        fresnelModels[i]->ActivateIrradianceMap(fboSystem->GetSkyboxRender(1), fboSystem->GetPrefilterRender(), fboSystem->GetSkyboxRender(2));

        for (int idLightD = 0; idLightD < LIMIT_DIR_CAST_SHADOW; idLightD++)
        {
            if (idLightD < this->shadowCastDirLights.size())
            {
                fresnelModels[i]->ActivateShadowMap(fboSystem->GetDirRender(idLightD), idLightD, TypeLight::DIRL);
            }
            else
            {
                fresnelModels[i]->ActivateShadowMap(NULL, idLightD, TypeLight::DIRL);
            }
        }

        for (int idLightS = 0; idLightS < LIMIT_SPOT_CAST_SHADOW; idLightS++)
        {
            if (idLightS < this->shadowCastSpotLights.size())
            {
                fresnelModels[i]->ActivateShadowMap(fboSystem->GetDirRender(idLightS), idLightS, TypeLight::SPOTL);
            }
            else
            {
                fresnelModels[i]->ActivateShadowMap(NULL, idLightS, TypeLight::SPOTL);
            }
        }

        for (int idLightP = 0; idLightP < LIMIT_OMNI_CAST_SHADOW; idLightP++)
        {
            if (idLightP < this->shadowCastOmniLights.size())
            {
                fresnelModels[i]->ActivateShadowMap(fboSystem->GetOmniRender(idLightP), idLightP, TypeLight::POINTLIGHT);
            }
            else
            {
                fresnelModels[i]->ActivateShadowMap(NULL, idLightP, TypeLight::POINTLIGHT);
            }
        }

        fresnelModels.at(i)->Render(lights, fboSystem->GetWaterRender(0), fboSystem->GetWaterRender(1));
    }
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

void RenderSystem::RenderParticleSystems()
{
    glDisable(GL_CULL_FACE);
    for (unsigned int i = 0; i < particleSystems.size(); i++)
    {
        particleSystems.at(i)->Render();
    }
    glEnable(GL_CULL_FACE);
}

void RenderSystem::PreRenderHDRSkybox()
{
    precookSkybox->PreRender();
    glViewport(0, 0, 512, 512);

    fboSystem->SkyboxProcessPass();
    //Terminamos de configurar el FBO a�adiendo las texturas del cubemap
    for (unsigned int i = 0; i < 6; ++i)
    {
        precookSkybox->matHandle->shader->setMat4("view", precookSkybox->captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, fboSystem->GetSkyboxRender(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        precookSkybox->DrawCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    fboSystem->SetIrradianceMap();

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    fboSystem->SkyboxProcessPass();

    precookSkybox->RenderIrradianceMap(cameras[0], fboSystem->GetSkyboxRender(), fboSystem->GetSkyboxRender(1));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    PrefilerPass();

    BRDFPass();
}

void RenderSystem::PrefilerPass()
{
    precookSkybox->SetUpPrefilterMap(fboSystem->GetSkyboxRender(), fboSystem->GetFBO(FBOType::PREFILTER_FBO));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::BRDFPass()
{
    fboSystem->GetFBO(FBOType::SKYBOX_FBO)->Set2DLUT();

    precookSkybox->DrawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::PreRender()
{
    SetRenderMode();

    //UNIFORM BUFFER OBJECT
    uboSytem = new UBOSystem();

    //SKYBOX
    skybox = new Skybox();
    skybox->AddCamera(cameras.at(0));

    //GUI
    renderPass.SetHDRParameters(guiSystem->HdrGui);
    renderPass.SetBloomParameters(guiSystem->bloomGui);
    renderPass.SetAtmScatParameters(guiSystem->atmGUI);
    guiSystem->SetLightInfoGui(&lights);
    guiSystem->SetCameraInfoGui(&cameras);
    guiSystem->SetModelInfoGui(&models);
    guiSystem->SetWaterModelInfoGui(&fresnelModels);

    //SET REFLECTIVE MATERIALS
    SetAmbientReflectiveMaterials();

    //SORTING MODELS
    for (unsigned int i = 0; i < models.size(); i++)
    {
        switch (models.at(i)->matHandle.type)
        {
            default:
            case MaterialType::EMISSIVE_LIT:
            case MaterialType::INSTANCE:
            case MaterialType::LIT:
            case MaterialType::UNLIT: 
                solidModels.push_back(models.at(i));
                break;
            case MaterialType::OUTLINE:
                outLineModels.push_back(models.at(i));
                break;
            case MaterialType::TRANSP:
                transparentModels.push_back(models.at(i));
                break;
            case MaterialType::FPS:
                fpsModels.push_back(models.at(i));
                break;
            case MaterialType::INTERNAL:
                internalModels.push_back(models.at(i));
                break;
        }
        //SET UBO BINDING SHADER
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shader->ID, "Matrices");
        if(models.at(i)->matHandle.type == MaterialType::OUTLINE)
            uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shader2->ID, "Matrices");

        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shaderPointShadow->ID, "Matrices");
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shaderShadow->ID, "Matrices");
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shaderVolumeShadow->ID, "Matrices");
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.occlussionShader->ID, "Matrices");
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.forwardVolumeShadowShader->ID, "Matrices");
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.depthMapShader->ID, "Matrices");

                                                                                                            //TEMPORAL
        for (int i = 0; i < pivot->GetModel()->colliders.size(); i++)
            uboSytem->SetUniformBlockIndex(pivot->GetModel()->colliders.at(i)->matHandle.shader->ID, "Matrices");

        renderPass.AddUBOSystem(uboSytem);
    }

    for (unsigned int i = 0; i < renderVolumes.size(); i++)
    {
        uboSytem->SetUniformBlockIndex(renderVolumes.at(i)->matHandle.shader->ID, "Matrices");
    }

    for (unsigned int i = 0; i < boundingModels.size(); i++)
    {
        uboSytem->SetUniformBlockIndex(boundingModels.at(i)->matHandle.shader->ID, "Matrices");
    }

    //CREAMOS UBO para VIEW & PROJECTION
    uboSytem->CreateBuffer((sizeof(glm::mat4) * 2) + sizeof(float), 0);

    //A�ADIMOS LUCES Y C�MARA
    for (unsigned int i = 0; i < models.size(); i++)
    {
        models.at(i)->AddLight(lights);
        models.at(i)->AddCamera(cameras.at(0));
    }

    for (unsigned int i = 0; i < renderVolumes.size(); i++)
    {
        renderVolumes.at(i)->AddCamera(cameras.at(0));
    }

    for (unsigned int i = 0; i < fresnelModels.size(); i++)
    {
        uboSytem->SetUniformBlockIndex(fresnelModels.at(i)->waterShader->ID, "Matrices");
        fresnelModels.at(i)->AddCamera(cameras.at(0));
        fresnelModels.at(i)->AddLights(lights);
    }

    for (unsigned int i = 0; i < particleSystems.size(); i++)
    {
        uboSytem->SetUniformBlockIndex(particleSystems.at(i)->particleShader->ID, "Matrices");
        particleSystems.at(i)->AddCamera(cameras.at(0));
        //fresnelModels.at(i)->AddLights(lights);
        particleSystems.at(i)->SetDeltaTime(&deltaTime);
    }


    //A�adimos la luz de scattering atmosf�rico
    if(!shadowCastOmniLights.empty())
        renderPass.AddAtmosphericScatteringLight(shadowCastOmniLights.at(0));

    //CREAMOS UN FRAME BUFFER OBJECT (FBO)
    glfwGetWindowSize(window, &width, &height);
    fboSystem = new FBOSystem(&width, &height);
    ///A�adimos MRT FBO
    fboSystem->AddFBO(new FBO(FBOType::MULT_RT, 8));
    ///A�adimos PINGPONG FBO
    fboSystem->AddFBO(new FBO(FBOType::PINGPONG_FBO));
    ///A�adimos Omnidirectional shadow FBO
    fboSystem->AddFBO(new FBO(FBOType::OMNI_SHADOW_FBO, 0, num_omni_cast_shadow));
    ///A�adimos Directional shadow FBO
    fboSystem->AddFBO(new FBO(FBOType::DIR_SHADOW_FBO, 0, num_dir_cast_shadow + num_spot_cast_shadow));
    ///A�adimos SSAO FBO
    fboSystem->AddFBO(new FBO(FBOType::SSAO_FBO));
    //A�adimos HDRskybox FBO
    fboSystem->AddFBO(new FBO(FBOType::SKYBOX_FBO));
    //A�adimos PrefilterSkybox FBO
    fboSystem->AddFBO(new FBO(FBOType::PREFILTER_FBO));
    //A�adimos Volume shadow FBO
    fboSystem->AddFBO(new FBO(FBOType::VOLUME_SHADOW_FBO));
    //A�adimos Light scattering FBO
    fboSystem->AddFBO(new FBO(FBOType::LIGHT_SCATTERING_FBO));
    //A�adimos Depth map FBO
    fboSystem->AddFBO(new FBO(FBOType::DEPTH_MAP_FBO));
    //A�adimos Water FBO
    fboSystem->AddFBO(new FBO(FBOType::WATER_FBO));

    //DEFERRED RENDER
    fboSystem->AddFBO(new FBO(FBOType::DEFFERED));
    renderPass.AddLights(lights);
    renderPass.AddCamera(cameras.at(0));
    renderPass.GenerateNoiseTexture();

    lastWidth = width;
    lastHeight = height;

    //INIT POST-PROCESS RENDER PLANE (Render Pass)
    renderPass.SetVAORenderPlane();
    renderPass.SetFBOSystem(fboSystem);

    //Cambiamos el numero de instancias en GPU del modelo
    //models.at(1)->matHandle.EditMaterial(MaterialComponent::NUM_INSTANCES, 10000.0f);

    ///ACTIVAMOS EL DEPTH BUFFER
    glEnable(GL_DEPTH_TEST);
    ///ACTIVAMOS EL CULLING
    glEnable(GL_CULL_FACE);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glEnable(GL_CLIP_DISTANCE0);

    //Preparamos el skybox
    if (precookSkybox != nullptr)
    { 
        PreRenderHDRSkybox();
    }
}

void RenderSystem::SetRenderMode()
{
    renderMode = guiSystem->GetRenderModeSelected();

    if (renderMode != RenderType::DEFERRED_RENDER)
    {
        if (guiSystem->isChangeShadowMode())
        {
            if (guiSystem->GetShadowMode() == ShadowType::SHADOW_MAP)
            {
                for (unsigned int i = 0; i < models.size(); i++)
                    models.at(i)->matHandle.ChangeCurrentShader(MaterialComponent::SHADER_FORWARD_QA);
            }
            else if (guiSystem->GetShadowMode() == ShadowType::SHADOW_VOL)
            {
                for (unsigned int i = 0; i < models.size(); i++)
                    models.at(i)->matHandle.ChangeCurrentShader(MaterialComponent::SHADER_VOLUME_SHADOW);
            }
            else
            {
                for (unsigned int i = 0; i < models.size(); i++)
                    models.at(i)->matHandle.ChangeCurrentShader(MaterialComponent::SHADER_FORWARD_QA);
            }
        }
    }

    for (unsigned int i = 0; i < models.size(); i++)
    {
        models.at(i)->matHandle.EditMaterial(RENDER_MODE, renderMode);
        models.at(i)->matHandle.EditMaterial(DRAW_MODE, guiSystem->GetDrawMode());
    }

    for (unsigned int i = 0; i < boundingModels.size(); i++)
        boundingModels.at(i)->matHandle.EditMaterial(RENDER_MODE, renderMode);
}

void RenderSystem::ForwardRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    ///UBO CAMERA
    uboSytem->StoreData(cameras.at(0)->projection, sizeof(glm::mat4));
    uboSytem->StoreData(cameras.at(0)->view, sizeof(glm::mat4), sizeof(glm::mat4));
    uboSytem->StoreData(glfwGetTime(), sizeof(float), sizeof(glm::mat4) * 2);

    if (guiSystem->isChangeShadowMode())
    {
        for (int idLight = 0; idLight < lights.size(); idLight++)
            lights.at(idLight)->SetShadowCastMode(guiSystem->GetShadowMode());

        for (int id = 0; id < solidModels.size(); id++)
            solidModels.at(id)->ChangeIndexSystem(guiSystem->GetShadowMode() == ShadowType::SHADOW_MAP || guiSystem->GetShadowMode() == ShadowType::SHADOW_CAS);
        for(int id = 0; id < outLineModels.size(); id++)
            outLineModels.at(id)->ChangeIndexSystem(guiSystem->GetShadowMode() == ShadowType::SHADOW_MAP || guiSystem->GetShadowMode() == ShadowType::SHADOW_CAS);
    }

    if (guiSystem->IsLightScattering())
    {
        fboSystem->LightScatteringPass();
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderOcclusionLightScattering();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    if (guiSystem->GetShadowMode() == ShadowType::SHADOW_MAP)
    {
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearStencil(0);
        glDepthMask(GL_TRUE);

        /// FIRST PASS -> RENDER DEPTH TO TEXTURE - DIRECTIONAL LIGHT
        //Render Directional Shadow Map
        RenderDirectionalShadowMap();
        /// FIRST PASS -> RENDER DEPTH TO TEXTURE - POINT LIGHT
        //Render OmniDirectional Shadow Map
        RenderOmnidirectionalShadowMap();
        /// SECOND PASS -> RENDER LIGHTING TO TEXTURE (MRT)
        fboSystem->MRTPass();
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        ///UBO CAMERA
        uboSytem->StoreData(cameras.at(0)->projection, sizeof(glm::mat4));
        uboSytem->StoreData(cameras.at(0)->view, sizeof(glm::mat4), sizeof(glm::mat4));
        uboSytem->StoreData(glfwGetTime(), sizeof(float), sizeof(glm::mat4) * 2);

        ///RENDER OUTLINE MODELS
        RenderOutLineModels();
        ///RENDER SOLID MATERIALS
        RenderSolidModels();
        ///RENDER SKYBOX
        //RenderSkyBox();
        ///RENDER TRANSPARENT MATERIALS
        //RenderTransparentModels();
        //RenderFPSModels();
        RenderInternalModels();
        ///MULTISAMPLING 
        fboSystem->MultisamplingPass();
        ///BLUR POST-PROCESS (BLOOM)
        renderPass.RenderBlur();
        /// FINAL PASS POST-PROCESS
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderPass.FinalRenderBloom();
    }
    else if (guiSystem->GetShadowMode() == ShadowType::SHADOW_VOL)
    {
        //FIRST PASS
        glViewport(0, 0, display_w, display_h);
        glClearStencil(0);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_TRUE);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        fboSystem->VolumeShadowPass();

        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        RenderSolidModels();
        RenderSkyBox();
        RenderOutLineModels();

        ///SECOND PASS
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboSystem->GetFBO(FBOType::VOLUME_SHADOW_FBO)->GetID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, display_w, display_h, 0, 0, display_w, display_h, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST);

        if (!guiSystem->isShowShadowVolumeMode())
        {
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDepthMask(GL_FALSE);
        }

        glEnable(GL_DEPTH_CLAMP);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_STENCIL_BUFFER_BIT);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xff);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);

        RenderVolumeShadow();

        glDisable(GL_DEPTH_CLAMP);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        ///THIRD PASS
        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_EQUAL, 0, 0xffff);

        renderPass.FinalRenderShadowVolume();

        glDisable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else
    {
    /*
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearStencil(0);
        glDepthMask(GL_TRUE);

        //Renderizamos Sombras
        RenderDirectionalShadowMap();
        RenderOmnidirectionalShadowMap();

        
        glEnable(GL_CLIP_DISTANCE0);
    
        //Reflection Pass
        //float distance_camera = 2 * (cameras[0]->cameraPos.y - 0.947f);
        float distance_camera = 2 * (cameras[0]->cameraPos.y - guiSystem->clipPlaneH);
        cameras[0]->InvertPitch(-distance_camera);
        uboSytem->StoreData(cameras.at(0)->view, sizeof(glm::mat4), sizeof(glm::mat4));

        fboSystem->WaterPass(0);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        RenderClipPlane(glm::vec4(0, 1, 0, guiSystem->clipPlaneH));
        RenderSkyBox();

        cameras[0]->InvertPitch(distance_camera);
        uboSytem->StoreData(cameras.at(0)->view, sizeof(glm::mat4), sizeof(glm::mat4));
        
        //Refraction Pass
        fboSystem->WaterPass(1);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        RenderClipPlane(glm::vec4(0, -1, 0, guiSystem->clipPlaneH));
        RenderSkyBox();

        glDisable(GL_CLIP_DISTANCE0);
    */    
        fboSystem->MRTPass();
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        RenderSolidModels();
        //RenderFresnelModels();
        RenderParticleSystems();
        //RenderSkyBox();
        
        ///MULTISAMPLING 
        fboSystem->MultisamplingPass();
        ///BLUR POST-PROCESS (BLOOM)
        renderPass.RenderBlur();
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderPass.FinalRenderBloom();
        
    }
}

void RenderSystem::DefferedRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    /// FIRST PASS -> RENDER DEPTH TO TEXTURE - DIRECTIONAL LIGHT
    //Render Directional Shadow Map
    RenderDirectionalShadowMap();

    /// FIRST PASS -> RENDER DEPTH TO TEXTURE - POINT LIGHT
    //Render OmniDirectional Shadow Map
    RenderOmnidirectionalShadowMap();

    /// SECOND PASS -> RENDER LIGHTING TO TEXTURE (MRT)
    fboSystem->DeferredGeometryPass();
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ///UBO CAMERA
    uboSytem->StoreData(cameras.at(0)->projection, sizeof(glm::mat4));
    uboSytem->StoreData(cameras.at(0)->view, sizeof(glm::mat4), sizeof(glm::mat4));
    uboSytem->StoreData(glfwGetTime(), sizeof(float), sizeof(glm::mat4) * 2);

    ///RENDER OUTLINE MODELS
    //RenderOutLineModels();
    ///RENDER SOLID MATERIALS
    RenderSolidModels();
    ///RENDER SKYBOX
    //RenderSkyBox();
    ///RENDER TRANSPARENT MATERIALS
    //RenderTransparentModels();

    /// THIRD PASS -> RENDER LIGHT VOLUMES
    //fboSystem->LightVolumePass();
    //glViewport(0, 0, display_w, display_h);
    //glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ///BLIT DEPTH BUFFER
    //fboSystem->BlitDepthBuffer(FBOType::DEFFERED, FBOType::LIGHTING_VOLUME_FBO);

    ///RENDER BOUNDING LIGHT MODELS
    //ProcessBoundingModels();

    ///MULTISAMPLING 
    //fboSystem->MultisamplingPass();

    ///BLUR POST-PROCESS (BLOOM)
    //renderPass.RenderBlur();

    ////SSAO
    //fboSystem->SSAOPass();
    //glClear(GL_COLOR_BUFFER_BIT);
    //renderPass.RenderSSAO();

    ////BLUR SSAO
    //fboSystem->SSAOPass(1); 
    //glClear(GL_COLOR_BUFFER_BIT);
    //renderPass.RenderBlurSSAO();

    /// FINAL PASS POST-PROCESS
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //renderPass.FinalRenderBloom();
    //renderPass.FinalRenderPass();
    renderPass.DefferedRender();
    
}

void RenderSystem::StartRender()
{
    while (!glfwWindowShouldClose(window) && !guiSystem->isShutDown())
    {
        //ANIMATION
        ComputeDeltaTime();
        GetWindowSize(window, &width, &height);

        //glm::vec3 tempPos = lights.at(0)->GetPosition();
        glm::vec3 tempPos = glm::vec3(0.0, 0.0, 0.0);
        glm::vec3 tempPos2 = glm::vec3(0.0, 1.0, 0.0);
        tempPos.x = sin(glfwGetTime() * 0.5) * 3.0;
        tempPos2.y = sin(glfwGetTime() * 0.5) * 3.0;

       /* models[1]->TranslationTo(tempPos);
        models[2]->TranslationTo(tempPos2);*/
        //lights.at(0)->EditLightComponent(LightComponent::LIGHT_POSITION, tempPos);

        //Set GPU instances
        for (int i = 0; i < models.size(); i++)
        {
            if (models.at(i)->matHandle.isChangeNumInstances && models.at(i)->matHandle.type == MaterialType::INSTANCE)
            {
                models.at(i)->SetIntanceModelMatrix();
                models.at(i)->matHandle.isChangeNumInstances = false;
            }
        }

        for (int i = 0; i < fresnelModels.size(); i++)
        {
            fresnelModels.at(i)->SetDelta(&deltaTime);
        }

        //Keyboad functions
        inputSystem.processInput(window); 
        //Mouse functions
        inputSystem.processMouseInput(window, cameras.at(0), models, pivot);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Draw UI
        guiSystem->DrawGUI();

        //Check Render Mode
        SetRenderMode();

        //CAMERA INPUT
        cameras.at(0)->CameraController(deltaTime);

        //UPDATE SIZE FBO
        UpdateFBO();

        // Rendering
        ImGui::Render();

        if (renderMode == RenderType::FORWARD_RENDER || renderMode == RenderType::FORWARD_QUALITY_RENDER)
            ForwardRender();
        else
            DefferedRender();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    Clean();
}

void RenderSystem::UpdateFBO()
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    width = display_w;
    height = display_h;

    if (lastWidth != width || lastHeight != height)
    { 
        fboSystem->ResizeFBOs();
    }

    lastWidth = width;
    lastHeight = height;

    if (guiSystem->msaaGui->isModifiedOffMSAA)
    {
        fboSystem->UpdateMRT(guiSystem->msaaGui->samplesOffScreenParameter);
    }

    if (guiSystem->msaaGui->isModifiedMSAA)
    {
        glfwWindowHint(GLFW_SAMPLES, guiSystem->msaaGui->samplesMSAAParameter);
    }

    int numPL, numSL, numDL;
    numPL = numSL = numDL = 0;

    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights.at(i)->isCastShadow)
        {
            switch (lights.at(i)->GetType())
            {
            case TypeLight::DIRL:
                numDL++;
                break;
            case TypeLight::POINTLIGHT:
                numPL++;
                break;
            case TypeLight::SPOTL:
                numSL++;
                break;
            }
        }
    }

    if (shadowCastGeneralLights.size() != numDL + numPL + numSL)
    {
        if (numPL != num_omni_cast_shadow)
        {
            shadowCastOmniLights.clear();
            fboSystem->UpdateOmniShadowFbo(numPL);
        }

        if (numDL != num_dir_cast_shadow || numSL != num_spot_cast_shadow)
        {
            if(numDL != num_dir_cast_shadow)
                shadowCastDirLights.clear();
            else
                shadowCastSpotLights.clear();

            fboSystem->UpdateDirShadowFbo(numDL + numSL);
        }

        shadowCastGeneralLights.clear();

        for (unsigned int i = 0; i < lights.size(); i++)
        {
            if (lights.at(i)->isCastShadow)
            {
                shadowCastGeneralLights.push_back(lights.at(i));

                switch (lights.at(i)->GetType())
                {
                    case TypeLight::DIRL:
                        if (numDL != num_dir_cast_shadow)
                            shadowCastDirLights.push_back(lights.at(i));
                        break;
                    case TypeLight::POINTLIGHT:
                        if (numPL != num_omni_cast_shadow)
                            shadowCastOmniLights.push_back(lights.at(i));
                        break;
                    case TypeLight::SPOTL:
                        if (numSL != num_spot_cast_shadow)
                            shadowCastSpotLights.push_back(lights.at(i));
                        break;
                }
            }
        }

        num_omni_cast_shadow = numPL;
        num_dir_cast_shadow = numDL;
        num_spot_cast_shadow = numSL;
    }
}

void RenderSystem::SetAmbientReflectiveMaterials()
{
    for (int i = 0; i < models.size(); i++)
    {
        if (models.at(i)->matHandle.isAmbientReflective)
        {
            for(int j = 0; j < models.at(i)->matHandle.listMaterials.size(); j++)
                models.at(i)->matHandle.listMaterials.at(j)->skyboxTexture = &(skybox->matHandle->listMaterials.at(0)->textures[0]);
        }
    }
}

void RenderSystem::SaveScene()
{
    if (guiSystem->isSave())
    {

    }
}
