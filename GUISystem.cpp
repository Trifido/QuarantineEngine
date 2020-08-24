#include "GUISystem.h"

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
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Example: Property editor", NULL);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        ImGui::Columns(2);
        ImGui::Separator();

        struct funcs
        {
            static void ShowDummyObject(const char* prefix, int uid)
            {
                ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
                ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
                bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
                ImGui::NextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text("my sailor is rich");
                ImGui::NextColumn();
                if (node_open)
                {
                    static float dummy_members[8] = { 0.0f,0.0f,1.0f,3.1416f,100.0f,999.0f };
                    for (int i = 0; i < 8; i++)
                    {
                        ImGui::PushID(i); // Use field index as identifier.
                        if (i < 2)
                        {
                            ShowDummyObject("Child", 424242);
                        }
                        else
                        {
                            // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                            ImGui::AlignTextToFramePadding();
                            ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Field_%d", i);
                            ImGui::NextColumn();
                            ImGui::SetNextItemWidth(-1);
                            if (i >= 5)
                                ImGui::InputFloat("##value", &dummy_members[i], 1.0f);
                            else
                                ImGui::DragFloat("##value", &dummy_members[i], 0.01f);
                            ImGui::NextColumn();
                        }
                        ImGui::PopID();
                    }
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        };

        // Iterate dummy objects with dummy members (all the same data)
        for (int obj_i = 0; obj_i < 3; obj_i++)
            funcs::ShowDummyObject("Object", obj_i);

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

                    
                    //for (int i = 0; i < 8; i++)
                    //{
                    //    ImGui::PushID(i); // Use field index as identifier.
                    //    if (i < 2)
                    //    {
                    //        ShowDummyObject("Child", 424242, light);
                    //    }
                    //    else
                    //    {
                    //        // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                    //        ImGui::AlignTextToFramePadding();
                    //        ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Field_%d", i);
                    //        ImGui::NextColumn();
                    //        ImGui::SetNextItemWidth(-1);
                    //        if (i >= 5)
                    //            ImGui::InputFloat("##value", &dummy_members[i], 1.0f);
                    //        else
                    //            ImGui::DragFloat("##value", &dummy_members[i], 0.01f);
                    //        ImGui::NextColumn();
                    //    }
                    //    ImGui::PopID();
                    //}

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


