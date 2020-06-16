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
    ssboSystem = new SSBOSystem();
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

    for (int i = 0; i < models.size(); i++)
    {
        models.at(i)->DeleteGPUInfo();
        //delete models.at(i);
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
        precookSkybox->precookFinal->use();
        precookSkybox->precookFinal->setInt("environmentMap", 0);
        precookSkybox->precookFinal->setMat4("projection", cameras[0]->projection);
        precookSkybox->precookFinal->setMat4("view", cameras[0]->view);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, fboSystem->GetSkyboxRender());
        precookSkybox->RenderHDR();
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
        glViewport(0, 0, 1024, 1024);
        glClear(GL_DEPTH_BUFFER_BIT);
               
        for (int idModel = 0; idModel < models.size(); idModel++)
            models[idModel]->DrawShadow(shadowCastDirLights.at(idLight)->lightSpaceMatrix);
    }
    int idLightGeneral = this->shadowCastDirLights.size();
    for (int idLight = idLightGeneral; idLight < this->shadowCastSpotLights.size(); idLight++)
    {
        fboSystem->DirShadowPass(idLight);
        glViewport(0, 0, 1024, 1024);
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
        glViewport(0, 0, 1024, 1024);
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

void RenderSystem::RenderSolidModels()
{
    glDisable(GL_CULL_FACE);
    for (int i = 0; i < solidModels.size(); i++)
    {
        if (solidModels[i]->CAST_SHADOW)
        {
            for (int idLight = 0; idLight < this->shadowCastDirLights.size(); idLight++)
                solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLight), idLight, TypeLight::DIRL);

            for (int idLight = 0; idLight < this->shadowCastSpotLights.size(); idLight++)
                solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetDirRender(idLight), idLight, TypeLight::SPOTL);

            for (int idLight = 0; idLight < this->shadowCastOmniLights.size(); idLight++)
                solidModels[i]->matHandle.ActivateShadowMap(fboSystem->GetOmniRender(idLight), idLight, TypeLight::POINTLIGHT);

            solidModels[i]->DrawCastShadow(shadowCastGeneralLights);
        }
        else
        {
            solidModels[i]->Draw();
        }
    }
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
        outLineModels[i]->Draw(true);
    }
}

void RenderSystem::PreRenderHDRSkybox()
{
    precookSkybox->PreRender();
    glViewport(0, 0, 512, 512);
    fboSystem->SkyboxProcessPass();
    //Terminamos de configurar el FBO añadiendo las texturas del cubemap
    for (unsigned int i = 0; i < 6; ++i)
    {
        //precookSkybox->matHandle->shader->use();
        precookSkybox->matHandle->shader->setMat4("view", precookSkybox->captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, fboSystem->GetSkyboxRender(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        precookSkybox->RenderHDR();
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR::SKYBOX Framebuffer not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::PreRender()
{
    SetRenderMode(RenderType::FORWARD_RENDER);
    //UNIFORM BUFFER OBJECT
    uboSytem = new UBOSystem();

    //SKYBOX
    skybox = new Skybox();
    skybox->AddCamera(cameras.at(0));

    //POST-PROCESS GAMMA
    renderPass.SetGamma(2.2f); 
    //TONE MAPPING 
    renderPass.SetExposure(0.5f);
    //BLOOM EFFECT
    renderPass.SetBloom(false);
    renderPass.SetAmountBloom(10);

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
        }
        //SET UBO BINDING SHADER
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shader->ID, "Matrices");
        if(models.at(i)->matHandle.type == MaterialType::OUTLINE)
            uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shader2->ID, "Matrices");

        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shaderPointShadow->ID, "Matrices");
        uboSytem->SetUniformBlockIndex(models.at(i)->matHandle.shaderShadow->ID, "Matrices");
        renderPass.AddUBOSystem(uboSytem);
    }

    for (unsigned int i = 0; i < boundingModels.size(); i++)
    {
        uboSytem->SetUniformBlockIndex(boundingModels.at(i)->matHandle.shader->ID, "Matrices");
    }

    //CREAMOS UBO para VIEW & PROJECTION
    uboSytem->CreateBuffer((sizeof(glm::mat4) * 2) + sizeof(float), 0);

    //AÑADIMOS LUCES
    for (unsigned int i = 0; i < models.size(); i++)
    {
        models.at(i)->AddLight(lights);
        models.at(i)->AddCamera(cameras.at(0));
    }

    //for (unsigned int i = 0; i < boundingModels.size(); i++)
    //{
        //boundingModels.at(i)->AddCamera(cameras.at(0));
        //boundingModels.at(i)->AddLight(lights);
    //}

    //CONFIGURAMOS Y AÑADIMOS SSBO
    ssboSystem->SetUpSSBOSystem();

    for (unsigned int i = 0; i < solidModels.size(); i++)
    {
        ssboSystem->ConnectSSBOToShader(solidModels.at(i)->matHandle.shader->ID);
    }

    //CREAMOS UN FRAME BUFFER OBJECT (FBO)
    glfwGetWindowSize(window, &width, &height);
    fboSystem = new FBOSystem(&width, &height);
    ///Añadimos MRT FBO
    fboSystem->AddFBO(new FBO(FBOType::MULT_RT, 8));
    ///Añadimos PINGPONG FBO
    fboSystem->AddFBO(new FBO(FBOType::PINGPONG_FBO));
    ///Añadimos Final FBO
    //fboSystem->AddFBO(new FBO(FBOType::COLOR_FBO, 16));
    ///Añadimos Omnidirectional shadow FBO
    fboSystem->AddFBO(new FBO(FBOType::OMNI_SHADOW_FBO, 0, num_omni_cast_shadow));
    ///Añadimos Directional shadow FBO
    fboSystem->AddFBO(new FBO(FBOType::DIR_SHADOW_FBO, 0, num_dir_cast_shadow + num_spot_cast_shadow));
    ///Añadimos Light Volume FBO
    //fboSystem->AddFBO(new FBO(FBOType::LIGHTING_VOLUME_FBO, 0));
    ///Añadimos SSAO FBO
    fboSystem->AddFBO(new FBO(FBOType::SSAO_FBO));
    //Añadimo HDRskybox FBO
    fboSystem->AddFBO(new FBO(FBOType::SKYBOX_FBO));

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

    //Preparamos el skybox
    if (precookSkybox != nullptr)
    { 
        PreRenderHDRSkybox();
    }
}

void RenderSystem::SetRenderMode(RenderType rmode)
{
    //assert(rmode == renderMode);
    renderMode = rmode;
    for (unsigned int i = 0; i < models.size(); i++)
        models.at(i)->matHandle.EditMaterial(RENDER_MODE, renderMode);

    for (unsigned int i = 0; i < boundingModels.size(); i++)
        boundingModels.at(i)->matHandle.EditMaterial(RENDER_MODE, renderMode);
}

void RenderSystem::ForwardRender()
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
    fboSystem->MRTPass();
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
    RenderSkyBox();
    ///RENDER TRANSPARENT MATERIALS
    //RenderTransparentModels();

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

void RenderSystem::DefferedRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);

    /// FIRST PASS -> RENDER DEPTH TO TEXTURE - DIRECTIONAL LIGHT
    //Render Directional Shadow Map
    //RenderDirectionalShadowMap();

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

    //SSAO
    fboSystem->SSAOPass();
    glClear(GL_COLOR_BUFFER_BIT);
    renderPass.RenderSSAO();

    //BLUR SSAO
    fboSystem->SSAOPass(1); 
    glClear(GL_COLOR_BUFFER_BIT);
    renderPass.RenderBlurSSAO();

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
    while (!glfwWindowShouldClose(window))
    {
        //ANIMATION
        ComputeDeltaTime();
        GetWindowSize(window, &width, &height);

        //ACTUALIZAMOS SSBO 
        ssboSystem->dataSSBO.camera_position = cameras.at(0)->cameraPos;
        ssboSystem->UpdateSSBOSystem();

        //glm::vec3 tempPos = lights.at(0)->GetPosition();
        //tempPos.x = sin(glfwGetTime() * 0.5) * 3.0;
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

        //Keyboad functions
        inputSystem.processInput(window); 
        //Mouse functions
        inputSystem.processMouseInput(window, cameras.at(0), models);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //CAMERA INPUT
        cameras.at(0)->CameraController(deltaTime);

        //UPDATE SIZE FBO
        UpdateFBO();

        // Rendering
        ImGui::Render();

        if (renderMode == RenderType::FORWARD_RENDER)
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
