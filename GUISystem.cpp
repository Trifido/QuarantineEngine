#include "GUISystem.h"

#include "RenderSystem.h"
#include "imgui.h"

GUISystem::GUISystem()
{
    bloomGui = new BloomGUI();
    HdrGui = new HdrGUI();
    msaaGui = new MsaaGUI();
}

void GUISystem::DrawGUI()
{
    DrawMainMenuBar();
}

void GUISystem::DrawMainMenuBar()
{
    static RenderType typeRender;
    static bool isShutdownRender = false;
    static bool isSaveSceneRender = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", NULL, &isShutdownRender))
            {
                isShutdownRender = true;
            }

            if (ImGui::MenuItem("Save Scene", NULL, &isSaveSceneRender))
            {
                isSaveSceneRender = true;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Render"))
        {
            if (ImGui::MenuItem("Forward Render", NULL))
            {
                typeRenderSelected = RenderType::FORWARD_RENDER;
                ///isActiveForwardRender = true;
                //isActiveDeferredRender = false;
            }
            if (ImGui::MenuItem("Forward PBR Render", NULL))
            {
                typeRenderSelected = RenderType::FORWARD_QUALITY_RENDER;
            }
            if (ImGui::MenuItem("Deferred Render", NULL))
            {
                typeRenderSelected = RenderType::DEFERRED_RENDER;
            }
            if (ImGui::BeginMenu("Render Mode"))
            {
                if (ImGui::MenuItem("Solid", NULL)) { draw_mode = DrawMode::DTRIANGLES; }
                if (ImGui::MenuItem("Wireframe", NULL)) { draw_mode = DrawMode::DLINES; }
                if (ImGui::MenuItem("Point", NULL)) { draw_mode = DrawMode::DPOINTS; }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Properties"))
        {
            if (ImGui::MenuItem("Model Properties", NULL)) { isOpenModelPropertyWindow = !isOpenModelPropertyWindow; }
            if (ImGui::MenuItem("Material Properties", NULL)) { isOpenMaterialPropertyWindow = !isOpenMaterialPropertyWindow; }
            if (ImGui::MenuItem("Light Properties", NULL)) { isOpenLightPropertyWindow = !isOpenLightPropertyWindow; }
            if (ImGui::MenuItem("Camera Properties", NULL)) { isOpenCameraPropertyWindow = !isOpenCameraPropertyWindow; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Post Process"))
        {
            if (ImGui::BeginMenu("Anti Aliasing"))
            {
                if (ImGui::MenuItem("MSAA")) isOpenMSAAWindow = !isOpenMSAAWindow;
                if (ImGui::MenuItem("Off-screen Multisampling")) isOpenOffScreeMSAAWindow = !isOpenOffScreeMSAAWindow;
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Bloom", NULL))
            {
                isOpenBloomWindow = !isOpenBloomWindow;
            }
            if (ImGui::MenuItem("HDR", NULL))
            {
                isOpenHDRWindow = !isOpenHDRWindow;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    DrawPostProcessWindow();
    DrawPropertyWindow();

    //typeRenderSelected = typeRender;
    isShutdown = isShutdownRender;
    isSaveScene = isSaveSceneRender;
}

void GUISystem::DrawPostProcessWindow()
{
    if (isOpenBloomWindow)
    {
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Bloom Post-process Tool");
        ImGui::Text("Bloom Controller");
        static bool enableBloomParameter = false;
        static int bloomRangeParameter = 20;
        ImGui::Checkbox("##circlesegmentoverride", &enableBloomParameter);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::SliderInt("Bloom Blur", &bloomRangeParameter, 0, 60);
        ImGui::End();

        SetBloomParameters(enableBloomParameter, bloomRangeParameter);
    }

    if (isOpenHDRWindow)
    {
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Post-process Tool");
        ImGui::Text("HDR & Gamma Controller");
        static float gammaParameter = 2.2f;
        static float exposureParameter = 0.5f;
        ImGui::SliderFloat("Gamma Correction", &gammaParameter, 0.0f, 5.0f);
        ImGui::SliderFloat("Tone Mapping Exposure", &exposureParameter, 0.0f, 10.0f);
        ImGui::End();

        SetHDRParameters(gammaParameter, exposureParameter);
    }

    if (isOpenOffScreeMSAAWindow)
    {
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Post-process Tool");
        ImGui::Text("Antialiasing Controller");
        static bool enableoffScreenSampleParameter = true;
        static int offScreenSampleParameter = 8;
        ImGui::Checkbox("##circlesegmentoverride", &enableoffScreenSampleParameter);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::SliderInt("Samples offscreen MSAA", &offScreenSampleParameter, 0, 32);
        ImGui::End();

        SetMSAAParameters(msaaGui->samplesMSAAParameter, offScreenSampleParameter, !enableoffScreenSampleParameter, enableoffScreenSampleParameter);
    }

    if (isOpenMSAAWindow)
    {
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Post-process Tool");
        ImGui::Text("Antialiasing GLFW Controller");
        static bool enableMSAASampleParameter = true;
        static int MSAASampleParameter = 8;
        ImGui::Checkbox("##circlesegmentoverride", &enableMSAASampleParameter);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::SliderInt("Samples GLFW MSAA", &MSAASampleParameter, 0, 32);
        ImGui::End();

        SetMSAAParameters(MSAASampleParameter, msaaGui->samplesOffScreenParameter, enableMSAASampleParameter, !enableMSAASampleParameter);
    }
}

void GUISystem::DrawPropertyWindow()
{
    if (isOpenModelPropertyWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(630, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Model Editor", NULL);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        ImGui::Columns(2);
        ImGui::Separator();

        struct funcs
        {
            static void ShowModelObject(const char* prefix, int uid, Model* model)
            {
                ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
                ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
                bool node_open = ImGui::TreeNode("Object", "%s", prefix);
                ImGui::NextColumn();
                ImGui::AlignTextToFramePadding();

                ImGui::NextColumn();
                if (node_open)
                {
                    //POSITION
                    ImGui::PushID(1);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldPosition", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", model->transform->RawPosition(), 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    //ROTATION
                    ImGui::PushID(2);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldRotation", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Rotation");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", model->transform->RawRotation(), 0.1f, -180.0f, 180.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    //Scale
                    ImGui::PushID(3);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldScale", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Scale");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", model->transform->RawScale(), 0.01f, 0.0f, 1000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    //IsSelectableModel
                    ImGui::PushID(4);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldSelectable", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Selectable");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("Selectable", &model->isSelectableModel);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::TreePop();
                }
                ImGui::PopID();

                model->transform->CheckChanges();
            }
        };

        // Iterate dummy objects with dummy members (all the same data)
        for (int obj_i = 0; obj_i < models->size(); obj_i++)
        {
            funcs::ShowModelObject(("Model_" + std::to_string(obj_i)).c_str(), obj_i, models->at(obj_i));
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
        ImGui::End();
    }

    if (isOpenMaterialPropertyWindow)
    {

    }

    if (isOpenLightPropertyWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(630, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Light Editor", NULL);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        ImGui::Columns(2);
        ImGui::Separator();

        struct funcs
        {
            static void ShowDummyObject(const char* prefix, int uid, Light* light)
            {
                ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
                ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
                bool node_open = ImGui::TreeNode("Object", "%s", prefix);
                ImGui::NextColumn();
                ImGui::AlignTextToFramePadding();

                switch (light->GetType())
                {
                case TypeLight::POINTLIGHT:
                    ImGui::Text("POINT LIGHT");
                    break;
                case TypeLight::DIRL:
                    ImGui::Text("DIRECTIONAL LIGHT");
                    break;
                case TypeLight::SPOTL:
                    ImGui::Text("SPOT LIGHT");
                    break;
                }
                
                ImGui::NextColumn();
                if (node_open)
                {
                    //POSITION
                    ImGui::PushID(1);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldPosition", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", light->GetRawPosition(), 0.01f, -100000.0f, 100000.0f);
                    light->EditLightComponent(LightComponent::LIGHT_POSITION, light->GetPosition());
                    ImGui::NextColumn();
                    ImGui::PopID();

                    //DIRECTION
                    if (light->GetType() != TypeLight::POINTLIGHT)
                    {
                        ImGui::PushID(2);
                        ImGui::AlignTextToFramePadding();
                        ImGui::TreeNodeEx("FieldDirection", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Direction");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(-1);
                        ImGui::DragFloat3("(X, Y, Z)", light->GetRawDirection(), 0.01f, -100000.0f, 100000.0f);
                        light->EditLightComponent(LightComponent::LIGHT_DIRECTION, light->GetDirection());
                        ImGui::NextColumn();
                        ImGui::PopID();

                    }

                    //DIFFUSE, SPECULAR & AMBIENT
                    ImGui::PushID(3);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldDiffuse", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Diffuse");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", light->GetRawDiffuse(), 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::PushID(4);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldSpecular", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Specular");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", light->GetRawSpecular(), 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::PushID(5);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldAmbient", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Ambient");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", light->GetRawAmbient(), 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    //CUTOFF & OUTERCUTOFF
                    if (light->GetType() == TypeLight::SPOTL)
                    {
                        ImGui::PushID(6);
                        ImGui::AlignTextToFramePadding();
                        ImGui::TreeNodeEx("FieldCutoff", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Cutoff");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(-1);
                        ImGui::DragFloat("", light->GetRawCutOff(), 0.01f, -100000.0f, 100000.0f);
                        ImGui::NextColumn();
                        ImGui::PopID();

                        ImGui::PushID(7);
                        ImGui::AlignTextToFramePadding();
                        ImGui::TreeNodeEx("FieldOuterCutoff", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Outer Cutoff");
                        ImGui::NextColumn();
                        ImGui::SetNextItemWidth(-1);
                        ImGui::DragFloat("", light->GetRawOuterCutOff(), 0.01f, -100000.0f, 100000.0f);
                        ImGui::NextColumn();
                        ImGui::PopID();
                    }

                    //LINEAR & QUADRATIC LIGHT DISTANCE ATTENUATION
                    ImGui::PushID(8);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldLinear", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Linear");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("", light->GetRawLinear(), 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::PushID(9);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldQuadratic", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Quadratic");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("", light->GetRawQuadratic(), 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    // CAST SHADOW
                    ImGui::PushID(10);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldCastShadow", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Cast Shadow");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::Checkbox("Cast Shadow", light->GetRawCastShadow());
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        };

        unsigned int pl, dl, sl;
        pl = sl = dl = 1;
        // Iterate dummy objects with dummy members (all the same data)
        for (int obj_i = 0; obj_i < lights->size(); obj_i++)
        {
            switch (lights->at(obj_i)->GetType())
            {
            case TypeLight::POINTLIGHT:
                funcs::ShowDummyObject(("Point_Light_" + std::to_string(pl)).c_str(), obj_i, lights->at(obj_i));
                pl++;
                break;
            case TypeLight::DIRL:
                funcs::ShowDummyObject(("Directional_Light_" + std::to_string(pl)).c_str(), obj_i, lights->at(obj_i));
                dl++;
                break;
            case TypeLight::SPOTL:
                funcs::ShowDummyObject(("Spot_Light_" + std::to_string(pl)).c_str(), obj_i, lights->at(obj_i));
                sl++;
                break;
            }
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
        ImGui::End();
    }

    if (isOpenCameraPropertyWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(630, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Camera Editor", NULL);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        ImGui::Columns(2);
        ImGui::Separator();

        struct funcs
        {
            static void ShowCameraObject(const char* prefix, int uid, Camera* camera)
            {
                ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
                ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
                bool node_open = ImGui::TreeNode("Object", "%s", prefix);
                ImGui::NextColumn();
                ImGui::AlignTextToFramePadding();

                float* position = new float[3];
                float* lookAt = new float[3];
                float* fov = new float();
                float* nearPlane = new float();
                float* farPlane = new float();
                position[0] = camera->cameraPos.x;
                position[1] = camera->cameraPos.y;
                position[2] = camera->cameraPos.z;
                lookAt[0] = camera->cameraFront.x;
                lookAt[1] = camera->cameraFront.y;
                lookAt[2] = camera->cameraFront.z;
                *fov = camera->GetFOV();
                *nearPlane = *camera->GetRawNearPlane();
                *farPlane = *camera->GetRawFarPlane();

                ImGui::NextColumn();
                if (node_open)
                {
                    //POSITION
                    ImGui::PushID(1);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldPosition", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Position");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", position, 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    //DIRECTION
                    ImGui::PushID(2);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldLookAT", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Look At");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat3("(X, Y, Z)", lookAt, 0.01f, -100000.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    //FOV
                    ImGui::PushID(3);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldFOV", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Field of View");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("", fov, 0.01f, 30.0f, 180.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();
                    //NEAR PLANE
                    ImGui::PushID(4);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldNEAR", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Near Plane");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("", nearPlane, 0.01f, 0.01f, 100.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();
                    //FAR PLANE
                    ImGui::PushID(5);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TreeNodeEx("FieldFar", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Far Plane");
                    ImGui::NextColumn();
                    ImGui::SetNextItemWidth(-1);
                    ImGui::DragFloat("", farPlane, 0.01f, 1.0f, 100000.0f);
                    ImGui::NextColumn();
                    ImGui::PopID();

                    ImGui::TreePop();
                }
                ImGui::PopID();

                camera->CheckCameraAttributes(position, lookAt, *fov, *nearPlane, *farPlane);
            }
        };

        // Iterate dummy objects with dummy members (all the same data)
        for (int obj_i = 0; obj_i < cameras->size(); obj_i++)
        {
            funcs::ShowCameraObject(("Camera_" + std::to_string(obj_i)).c_str(), obj_i, cameras->at(obj_i));
        }

        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::PopStyleVar();
        ImGui::End();
    }
}

RenderType GUISystem::GetRenderModeSelected()
{
    return typeRenderSelected;
}

bool GUISystem::isShutDown()
{
    return isShutdown;
}

bool GUISystem::isSave()
{
    return isSaveScene;
}

void GUISystem::SetBloomParameters(bool enableParam, int rangeParam)
{
    bloomGui->enable = enableParam;
    bloomGui->blurRange = rangeParam;
}

void GUISystem::SetHDRParameters(float gammaParam, float exposureParam)
{
    HdrGui->gammaParameter = gammaParam;
    HdrGui->exposureParameter = exposureParam;
}

void GUISystem::SetMSAAParameters(unsigned int msaaSamples, unsigned int offSamples, bool enableMSAA, bool enableOffMSAA)
{
    if (!enableMSAA) msaaSamples = 0;
    if (!enableOffMSAA) offSamples = 0;

    if (msaaSamples != msaaGui->samplesMSAAParameter)
        msaaGui->isModifiedMSAA = true;
    else
        msaaGui->isModifiedMSAA = false;

    if (offSamples != msaaGui->samplesOffScreenParameter)
        msaaGui->isModifiedOffMSAA = true;
    else
        msaaGui->isModifiedOffMSAA = false;

    msaaGui->enableMSAA = enableMSAA;
    msaaGui->enableOffMSAA = enableOffMSAA;
    msaaGui->samplesMSAAParameter = msaaSamples;
    msaaGui->samplesOffScreenParameter = offSamples;
}

void GUISystem::SetLightInfoGui(std::vector<Light*>* lights)
{
    this->lights = lights;
}

void GUISystem::SetCameraInfoGui(std::vector<Camera*>* cameras)
{
    this->cameras = cameras;
}

void GUISystem::SetModelInfoGui(std::vector<Model*>* models)
{
    this->models = models;
}


