#include "RenderSystem.h"

RenderSystem::RenderSystem(GLFWwindow* window, ImVec4* clearColor)
{
    deltaTime = lastFrame = 0.0f;
    this->window = window;
    clear_color = clearColor;
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
}

void RenderSystem::AddCamera(Camera* cam)
{
    this->cameras.push_back(cam);
}

void RenderSystem::AddModel(Model* model)
{
    this->models.push_back(model);
}

void RenderSystem::RenderSkyBox()
{
    skybox->Render();
}

void RenderSystem::RenderSolidModels()
{
    for (int i = 0; i < solidModels.size(); i++)
    {
        solidModels[i]->Draw(true);
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

void RenderSystem::RenderOutLineModels()
{
    for (int i = 0; i < outLineModels.size(); i++)
    {
        outLineModels[i]->Draw(true);
    }
}

void RenderSystem::PreRender()
{
    //UNIFORM BUFFER OBJECT
    uboSytem = new UBOSystem();

    //SKYBOX
    skybox = new Skybox();
    skybox->AddCamera(cameras.at(0));

    //SET REFLECTIVE MATERIALS
    SetAmbientReflectiveMaterials();

    //SORTING MODELS
    for (unsigned int i = 0; i < models.size(); i++)
    {
        switch (models.at(i)->matHandle.type)
        {
            default:
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
    }

    //CREAMOS UBO para VIEW & PROJECTION
    uboSytem->CreateBuffer((sizeof(glm::mat4) * 2) + sizeof(float), 0);

    //A�ADIMOS LUCES
    for (unsigned int i = 0; i < models.size(); i++)
    {
        models.at(i)->AddLight(lights);
        models.at(i)->AddCamera(cameras.at(0));
    }

    //INIT POST-PROCESS RENDER PLANE
    renderFinalPass.SetVAORenderPlane();

    //CREAMOS UN FRAME BUFFER OBJECT FBO
    glfwGetWindowSize(window, &width, &height);
    fboSystem = new FBOSystem(&width, &height, 4);

    lastWidth = width;
    lastHeight = height;

    fboSystem->InitFBOSystem();

    //Cambiamos el numero de instancias en GPU del modelo
    //models.at(1)->matHandle.EditMaterial(MaterialComponent::NUM_INSTANCES, 10000.0f);

    ///ACTIVAMOS EL ANTIALIASING
    glEnable(GL_MULTISAMPLE);

    ///ACTIVAMOS EL DEPTH BUFFER
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    ///ACTIVAMOS EL CULLING
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}

void RenderSystem::StartRender()
{
    while (!glfwWindowShouldClose(window))
    {
        //ANIMATION
        ComputeDeltaTime();
        GetWindowSize(window, &width, &height);

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
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // FIRST PASS
        fboSystem->ActivateFBORender();

        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //UBO CAMERA
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
        RenderTransparentModels();

        //SET FBO MULTISAMPLING
        fboSystem->SetMultiSamplingFrameBuffer();

        // SECOND PASS
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        renderFinalPass.DrawFrom(fboSystem->texColorBuffer);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    Clean();
}

void RenderSystem::UpdateFBO()
{
    if (lastWidth != width || lastHeight != height)
        fboSystem->InitFBOSystem();

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
