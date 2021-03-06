#include "Requisites.h"

#include <Mmsystem.h>
#include <mciapi.h>
#include <thread>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")

void PlayBSO()
{
    PlaySound("./resources/soundtrack/base.wav", NULL, SND_ASYNC);
}

int main(int, char**)
{
    std::thread first(PlayBSO);

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.5);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Quarantine Engine", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //ImVec4* clear_color = new ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4* clear_color = new ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    //DEMO
    DemoLogic* demo = new DemoLogic();
    //PHYSIC SYSTEM
    PhysicSystem physickernel;
    //RENDER SYSTEM
    RenderSystem renderkernel(window, clear_color);
    renderkernel.AddPhysicSystem(&physickernel);
    renderkernel.AddDemo(demo);
    ///TEXTURES
    std::vector<Texture> textures;
    //Texture texture1("resources/brickwall.jpg", TypeTexture::DIFFUSE, true);
    //Texture texture2("resources/brickwall_normal.jpg", TypeTexture::NORMAL); 
    //Texture texture3("resources/matrix.jpg", TypeTexture::EMISSIVE);
    //Texture texture4("./resources/grass.png", TypeTexture::DIFFUSE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    //textures.push_back(texture1);
    //textures.push_back(texture2);
    //textures.push_back(texture3);
    //Texture textureFloor("./resources/wood.png", TypeTexture::DIFFUSE, false);
    //std::vector<Texture> textures_floor;
    //textures_floor.push_back(textureFloor);

    //Texture parallaxDiff("resources/materials/floor_dirt/Stone_Path_001_baseColor.jpg", TypeTexture::DIFFUSE);
    //Texture parallaxNorm("resources/materials/floor_dirt/Stone_Path_001_normal.jpg", TypeTexture::NORMAL);
    //Texture parallaxDisp("resources/materials/floor_dirt/Stone_Path_001_height.png", TypeTexture::HEIGHT);
    //Texture parallaxRoug("resources/materials/floor_dirt/Stone_Path_001_roughness.jpg", TypeTexture::ROUGHNESS);
    //Texture parallaxAO("resources/materials/floor_dirt/Stone_Path_001_ambientOcclusion.jpg", TypeTexture::AO);

    //std::vector<Texture> textures_parallax;
    //textures_parallax.push_back(parallaxDiff);
    //textures_parallax.push_back(parallaxNorm);
    //textures_parallax.push_back(parallaxDisp);
    //textures_parallax.push_back(parallaxRoug);
    //textures_parallax.push_back(parallaxAO);


    std::vector<Texture> textures_pbr;
    Texture pbrDiff("resources/PBR_Textures/rustedIron/rustediron2_basecolor.png", TypeTexture::DIFFUSE);
    Texture pbrNorm("resources/PBR_Textures/rustedIron/rustediron2_normal.png", TypeTexture::NORMAL);
    Texture pbrME("resources/PBR_Textures/rustedIron/rustediron2_metallic.png", TypeTexture::METALLIC);
    Texture pbrRO("resources/PBR_Textures/rustedIron/rustediron2_roughness.png", TypeTexture::ROUGHNESS);
    Texture pbrAO("resources/PBR_Textures/rustedIron/rustediron2_roughness.png", TypeTexture::AO);

    //Texture pbrDiff("resources/PBR_Textures/ironKiwi/New_Graph_diffuse.jpg", TypeTexture::DIFFUSE);
    //Texture pbrNorm("resources/PBR_Textures/ironKiwi/New_Graph_normal.jpg", TypeTexture::NORMAL);
    //Texture pbrME("resources/PBR_Textures/ironKiwi/New_Graph_specular.jpg", TypeTexture::METALLIC);
    //Texture pbrRO("resources/PBR_Textures/ironKiwi/New_Graph_glossiness.jpg", TypeTexture::ROUGHNESS);
    //Texture pbrBUMP("resources/PBR_Textures/ironKiwi/New_Graph_height.jpg", TypeTexture::HEIGHT);
    //Texture pbrAO("resources/PBR_Textures/ironKiwi/rustediron2_roughness.png", TypeTexture::AO);

    textures_pbr.push_back(pbrDiff);
    textures_pbr.push_back(pbrNorm);
    textures_pbr.push_back(pbrME);
    textures_pbr.push_back(pbrRO);
    //textures_pbr.push_back(pbrBUMP);
    textures_pbr.push_back(pbrAO);

    //std::vector<Texture> textures_pbr_floor;
    //Texture floor_pbrDiff("resources/materials/metal/Metal_Plate_013_basecolor.jpg", TypeTexture::DIFFUSE);
    //Texture floor_pbrNorm("resources/materials/metal/Metal_Plate_013_normal.jpg", TypeTexture::NORMAL);
    //Texture floor_pbrME("resources/materials/metal/Metal_Plate_013_metallic.jpg", TypeTexture::METALLIC);
    //Texture floor_pbrRO("resources/materials/metal/Metal_Plate_013_roughness.jpg", TypeTexture::ROUGHNESS);
    //Texture floor_pbrAO("resources/materials/metal/Metal_Plate_013_ambientOcclusion.jpg", TypeTexture::AO);
    //Texture floor_pbrHE("resources/materials/metal/Metal_Plate_013_height.png", TypeTexture::HEIGHT);

    //textures_pbr_floor.push_back(floor_pbrDiff);
    //textures_pbr_floor.push_back(floor_pbrNorm);
    //textures_pbr_floor.push_back(floor_pbrME);
    //textures_pbr_floor.push_back(floor_pbrRO);
    //textures_pbr_floor.push_back(floor_pbrAO);
    //textures_pbr_floor.push_back(floor_pbrHE);

    ///SHADERS
    //Shader* outlineShader = new Shader("shaders/outline.vert", "shaders/outline.frag");
    //Shader* transparentShader = new Shader("shaders/blending.vert", "shaders/blending.frag");
    //Shader* refractiveShader = new Shader("shaders/refractive.vert", "shaders/refractive.frag");
    //Shader* refractiveNanoShader = new Shader("shaders/refractive.vert", "shaders/refractive.frag");
    //Shader* reflectiveShader = new Shader("shaders/reflective.vert", "shaders/reflective.frag");
    ////Shader* geometryShader = new Shader("shaders/geometryShader.vert", "shaders/geometryShader.gm", "shaders/geometryShader.frag");
    //Shader* geometryShader2 = new Shader("shaders/nanosuit.vert", "shaders/geometryShaderNano.gm", "shaders/nanosuit.frag");
    //Shader* geometryShaderNormal = new Shader("shaders/geometryShaderNormal.vert", "shaders/geometryShaderNormal.gm", "shaders/geometryShaderNormal.frag");
    //Shader* instanceShader = new Shader("shaders/instancing_quads.vert", "shaders/instancing_quads.frag");
    //Shader* standardShader = new Shader("shaders/standardLighting.vert", "shaders/standardLighting.frag");
    //Shader* instancingShader = new Shader("shaders/instancing.vert", "shaders/instancing.frag");
    //Shader* renderShader = new Shader("shaders/standardShadow.vert", "shaders/standardShadow.frag");
    //Shader* emissiveBloomShader = new Shader("shaders/emissive.vert", "shaders/emissive.frag");
    ///MATERIALES
    /*
    Material *cubeMaterial = new Material(refractiveShader, outlineShader, textures);
    Material *nanosuitMaterial = new Material(geometryShader2);
    Material* transpVegi = new Material(transparentShader);
    Material* normalMaterial = new Material(standardShader, geometryShaderNormal, MaterialType::NORMALS);
    Material* instanceMaterial = new Material(instanceShader);
    //Material* geoMaterial = new Material(geometryShader);
    cubeMaterial->shininess = 12.0f;

    textures.clear();
    textures.push_back(texture4);
    transpVegi->AddMultTextures(textures);
    transpVegi->type = MaterialType::TRANSP;
    transpVegi->shininess = 12.0f;
    //Material* mat_asteroid = new Material(instancingShader);
    */ 
    //Material* emissiveMaterial = new Material(emissiveBloomShader);
    //emissiveMaterial->type = MaterialType::EMISSIVE_LIT;
    ///CAMERA
    Camera mainCamera(1280, 720);
    renderkernel.AddCamera(&mainCamera);

    ///MODEL 3D
    ///--NANOSUIT  
    //Model ourModel("./resources/morgue/window/model.dae");
    //Model ourModel("./resources/3DModels/Mountain_Snow.fbx");
    //Model ourModel("./resources/3DModels/colt/model.obj");
    //Model ourModel("./resources/cave/PLATFORM/platform.obj");
    //Model ourModel("./resources/3DModels/fps_m16/dae/arm.fbx");
    //Model ourModel("./resources/space/station/SpaceShip.obj");
    //Model ourModel("./resources/3DModels/sponza/sponza.obj");
    //Model ourModel("./resources/3DModels/crysis/nanosuit.obj");
    //Model ourModel("./resources/3DModels/sphere/sphere.obj");
    //Model ourModel("./resources/3DModels/head/head.obj"); 
    //ourModel.matHandle.EditMaterial(MaterialComponent::SHADER1, renderShader);
    /*ourModel.AddMaterial(normalMaterial);
    //ourModel.matHandle.EditMaterial(MaterialComponent::A_REFRACTIVE, true);
    //ourModel.matHandle.EditMaterial(MaterialComponent::REFRACTIVE_INDEX, 1.52f);
    */
    //ourModel.ScaleTo(glm::vec3(0.005f));
    //ourModel.transform->model *= glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    //ourModel.transform->model *= glm::rotate(glm::mat4(1.0), glm::radians(-10.0f), glm::vec3(1.0, 0.0, 0.0));
    //ourModel.Rotation(glm::radians(180.0f),glm::vec3(0.0, 1.0, 0.0));
    //ourModel.transform->model = glm::translate(ourModel.transform->model, glm::vec3(-2.5, 0.0, 5.0));
    //ourModel.TranslationTo(glm::vec3(-2.0, 0.0, 0.0));

    //ourModel.matHandle.EditMaterial(MaterialComponent::TEXTURE, textures_pbr);
    //renderkernel.AddModel(&ourModel);

    //DEMO
    //Platform
    /**/
    Model platform1("./resources/cave/PLATFORM/platform.obj");
    platform1.RotationTo(0.0f, glm::vec3(0.0, 1.0, 0.0));
    platform1.ScaleTo(glm::vec3(0.005f));
    platform1.transform->model = glm::translate(platform1.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&platform1);
    Model platform2("./resources/cave/PLATFORM/platform.obj");
    platform2.RotationTo(90.0f, glm::vec3(0.0, 1.0, 0.0));
    platform2.ScaleTo(glm::vec3(0.005f));
    platform2.transform->model = glm::translate(platform2.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&platform2);
    Model platform3("./resources/cave/PLATFORM/platform.obj");
    platform3.RotationTo(180.0f, glm::vec3(0.0, 1.0, 0.0));
    platform3.ScaleTo(glm::vec3(0.005f));
    platform3.transform->model = glm::translate(platform3.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&platform3);
    Model platform4("./resources/cave/PLATFORM/platform.obj");
    platform4.RotationTo(270.0f, glm::vec3(0.0, 1.0, 0.0));
    platform4.ScaleTo(glm::vec3(0.005f));
    platform4.transform->model = glm::translate(platform4.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&platform4);

    //Columns
    Model columns1("./resources/cave/COLUMNS/columns.obj");
    columns1.RotationTo(0.0f, glm::vec3(0.0, 1.0, 0.0));
    columns1.ScaleTo(glm::vec3(0.005f));
    columns1.transform->model = glm::translate(columns1.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&columns1);
    Model columns2("./resources/cave/COLUMNS/columns.obj");
    columns2.RotationTo(90.0f, glm::vec3(0.0, 1.0, 0.0));
    columns2.ScaleTo(glm::vec3(0.005f));
    columns2.transform->model = glm::translate(columns2.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&columns2);
    Model columns3("./resources/cave/COLUMNS/columns.obj");
    columns3.RotationTo(180.0f, glm::vec3(0.0, 1.0, 0.0));
    columns3.ScaleTo(glm::vec3(0.005f));
    columns3.transform->model = glm::translate(columns3.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&columns3);
    Model columns4("./resources/cave/COLUMNS/columns.obj");
    columns4.RotationTo(270.0f, glm::vec3(0.0, 1.0, 0.0));
    columns4.ScaleTo(glm::vec3(0.005f));
    columns4.transform->model = glm::translate(columns4.transform->model, glm::vec3(0.0f, -100.5f, 0.0f));
    renderkernel.AddModel(&columns4);

    //Dome
    Model dome1("./resources/cave/DOME/dome.obj");
    dome1.RotationTo(0.0f, glm::vec3(0.0, 1.0, 0.0));
    dome1.ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(&dome1);
    Model dome2("./resources/cave/DOME/dome.obj");
    dome2.RotationTo(180.0f, glm::vec3(0.0, 1.0, 0.0));
    dome2.ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(&dome2);
    
    /**/
    //Outside
    Model columnsDestroyed("./resources/cave/COLUMNS_DESTROYED/outside.obj");
    columnsDestroyed.ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(&columnsDestroyed);
    /**/
    //Rock Wall
    Model mainCave("./resources/cave/MAIN_CAVE/maincave.obj");
    mainCave.ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(&mainCave);
    
    //Ground MainCave
    std::vector<Texture> textures_pbr_floor;
    Texture floor_pbrDiff("./resources/cave/GROUND_CAVE/brown_mud_rocks_01_diff_2k.png", TypeTexture::DIFFUSE);
    Texture floor_pbrNorm("./resources/cave/GROUND_CAVE/brown_mud_rocks_01_nor_2k.png", TypeTexture::NORMAL);
    Texture floor_pbrME("./resources/cave/GROUND_CAVE/brown_mud_rocks_01_spec_2k.png", TypeTexture::METALLIC);
    Texture floor_pbrRO("./resources/cave/GROUND_CAVE/brown_mud_rocks_01_rough_2k.png", TypeTexture::ROUGHNESS);
    Texture floor_pbrAO("./resources/cave/GROUND_CAVE/brown_mud_rocks_01_AO_2k.png", TypeTexture::AO);
    Texture floor_pbrHE("./resources/cave/GROUND_CAVE/brown_mud_rocks_01_disp_2k.png", TypeTexture::HEIGHT);
    textures_pbr_floor.push_back(floor_pbrDiff);
    textures_pbr_floor.push_back(floor_pbrNorm);
    textures_pbr_floor.push_back(floor_pbrME);
    textures_pbr_floor.push_back(floor_pbrRO);
    textures_pbr_floor.push_back(floor_pbrAO);
    textures_pbr_floor.push_back(floor_pbrHE);

    Model groundMainCave(floorVertices, 6, textures_pbr_floor);
    groundMainCave.SetPositionOrientation(reactphysics3d::Vector3(0.0f, -1.080, -2.150));
    groundMainCave.SetPhysicType(PhysicBodyType::RIGIDBODY_STATIC);
    groundMainCave.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(24.f, 0.2f, 24.f));
    groundMainCave.matHandle.EditMaterial(MaterialComponent::BLOOM_BRIGHTNESS, 100.0f);
    groundMainCave.matHandle.EditMaterial(MaterialComponent::P_DISPLACEMENT, -0.1f);
    groundMainCave.matHandle.EditMaterial(MaterialComponent::MIN_UV, 0.0f);
    groundMainCave.matHandle.EditMaterial(MaterialComponent::MAX_UV, 10.0f);
    //groundMainCave.transform->model = glm::translate(groundMainCave.transform->model, glm::vec3(0.0, -1.080, -2.150));
    groundMainCave.ScaleTo(glm::vec3(2.670f, 1.0f, 2.080f));
    renderkernel.AddModel(&groundMainCave);

    //Ground river
    std::vector<Texture> textures_pbr_floor_river;
    Texture floor_river_pbrDiff("./resources/cave/GROUND_RIVER/MI_Cave_Rock_Flat_MI_Cave_Rock_Flat_D.bmp", TypeTexture::DIFFUSE);
    Texture floor_river_pbrNorm("./resources/cave/GROUND_RIVER/MI_Cave_Rock_Flat_MI_Cave_Rock_Flat_N.bmp", TypeTexture::NORMAL);
    Texture floor_river_pbrRO("./resources/cave/GROUND_RIVER/MI_Cave_Rock_Flat_MI_Cave_Rock_Flat_S.bmp", TypeTexture::ROUGHNESS);
    textures_pbr_floor_river.push_back(floor_river_pbrDiff);
    textures_pbr_floor_river.push_back(floor_river_pbrNorm);
    textures_pbr_floor_river.push_back(floor_river_pbrRO);

    Model groundRiverCave(floorVertices, 6, textures_pbr_floor_river);
    groundRiverCave.SetPositionOrientation(reactphysics3d::Vector3(-12.210f, -2.140f, 35.970f));
    groundRiverCave.SetPhysicType(PhysicBodyType::RIGIDBODY_STATIC);
    groundRiverCave.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(20.f, 0.001f, 20.f));
    groundRiverCave.matHandle.EditMaterial(MaterialComponent::BLOOM_BRIGHTNESS, 100.0f);
    groundRiverCave.matHandle.EditMaterial(MaterialComponent::MIN_UV, 0.0f);
    groundRiverCave.matHandle.EditMaterial(MaterialComponent::MAX_UV, 20.0f);
    //groundRiverCave.transform->model = glm::translate(groundRiverCave.transform->model, glm::vec3(-12.210f, -1.070f, 35.970f));
    //groundRiverCave.transform->model = glm::translate(groundRiverCave.transform->model, glm::vec3(0.0f, -1.070f, 0.0f));
    groundRiverCave.ScaleTo(glm::vec3(3.210f, 1.0f, 1.740f));
    renderkernel.AddModel(&groundRiverCave);
    
    //BRICK WALL
    Model brick1("./resources/cave/BRICK/brick.obj");
    brick1.SetPositionOrientation(reactphysics3d::Vector3(1.130f, -2.010f, 22.520f), rp3d::Quaternion(0.0f, 0.85f, 0.0f, 1.0f));
    brick1.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    brick1.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(1.1, 0.65, 1.3));
    brick1.ScaleTo(glm::vec3(0.05f));
    renderkernel.AddModel(&brick1);

    Model brick2("./resources/cave/BRICK/brick.obj");
    brick2.SetPositionOrientation(reactphysics3d::Vector3(4.870f, -2.010f, 22.6f), rp3d::Quaternion(0.0f, 0.85f, 0.0f, 1.0f));
    brick2.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    brick2.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(1.1, 0.65, 1.3));
    brick2.ScaleTo(glm::vec3(0.05f));
    renderkernel.AddModel(&brick2);

    Model brick3("./resources/cave/BRICK/brick.obj");
    brick3.SetPositionOrientation(reactphysics3d::Vector3(1.77f, -0.72f, 21.920f), rp3d::Quaternion(0.0f, 0.85f, 0.0f, 1.0f));
    brick3.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    brick3.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(1.1, 0.65, 1.3));
    brick3.ScaleTo(glm::vec3(0.05f));
    renderkernel.AddModel(&brick3);

    Model brick4("./resources/cave/BRICK/brick.obj");
    brick4.SetPositionOrientation(reactphysics3d::Vector3(3.920f, -0.72f, 22.520f), rp3d::Quaternion(0.0f, 0.85f, 0.0f, 1.0f));
    brick4.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    brick4.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(1.1, 0.65, 1.3));
    brick4.ScaleTo(glm::vec3(0.05f));
    renderkernel.AddModel(&brick4);

    Model brick5("./resources/cave/BRICK/brick.obj");
    brick5.SetPositionOrientation(reactphysics3d::Vector3(2.520f, 1.f, 22.520f), rp3d::Quaternion(0.0f, 0.85f, 0.0f, 1.0f));
    brick5.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    brick5.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(1.1, 0.65, 1.3));
    brick5.ScaleTo(glm::vec3(0.05f));
    renderkernel.AddModel(&brick5);

    Model brick6("./resources/cave/BRICK/brick.obj");
    brick6.SetPositionOrientation(reactphysics3d::Vector3(4.920f, 1.f, 22.520f), rp3d::Quaternion(0.0f, 0.85f, 0.0f, 1.0f));
    brick6.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    brick6.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(1.1, 0.65, 1.3));
    brick6.ScaleTo(glm::vec3(0.05f));
    renderkernel.AddModel(&brick6);

    Model brick7("./resources/cave/BRICK/brick.obj");
    brick7.SetPositionOrientation(reactphysics3d::Vector3(3.820f, 1.7f, 22.520f), rp3d::Quaternion(0.0f, 0.85f, 0.0f, 1.0f));
    brick7.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    brick7.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(1.1, 0.65, 1.3));
    brick7.ScaleTo(glm::vec3(0.05f));
    renderkernel.AddModel(&brick7);
    
    //Water river
    Water *waterPlane = new Water();
    *waterPlane->GetWaveSpeed() = 0.2f;
    *waterPlane->GetTiling() = 20;
    waterPlane->GetWaterColor()[0] = 7.f / 256.f;
    waterPlane->GetWaterColor()[1] = 10.f / 256.f;
    waterPlane->GetWaterColor()[2] = 7.f / 256.f;
    *waterPlane->GetWaterColorFactor() = 0.316f;
    *waterPlane->GetWaterRefractiveFactor() = 0.368f;
    renderkernel.AddFresnelModel(waterPlane);
    /**/
    //Particle System
    ParticleSystem* text1 = new ParticleSystem(ParticleSystemType::BILLBOARD,"./resources/cave/TEXT/text0.png");
    text1->isText = true;
    text1->SetPosition(glm::vec3(10.39f, 0.92f, 32.2f));
    renderkernel.AddParticleSystem(text1);
    ParticleSystem* text2 = new ParticleSystem(ParticleSystemType::BILLBOARD,"./resources/cave/TEXT/text1.png");
    text2->isText = true;
    text2->SetPosition(glm::vec3(10.39f, 0.92f, 32.2f));
    renderkernel.AddParticleSystem(text2);
    ParticleSystem* text3 = new ParticleSystem(ParticleSystemType::BILLBOARD,"./resources/cave/TEXT/text2.png");
    text3->isText = true;
    text3->SetPosition(glm::vec3(-25.250f, 0.5f, 42.910));
    renderkernel.AddParticleSystem(text3);
    ParticleSystem* text4 = new ParticleSystem(ParticleSystemType::BILLBOARD,"./resources/cave/TEXT/text3.png");
    text4->isText = true;
    text4->SetPosition(glm::vec3(10.39f, 0.92f, 32.2f));
    renderkernel.AddParticleSystem(text4);
    ParticleSystem* text5 = new ParticleSystem(ParticleSystemType::BILLBOARD,"./resources/cave/TEXT/text4.png");
    text5->isText = true;
    text5->SetPosition(glm::vec3(10.39f, 0.92f, 32.2f));
    renderkernel.AddParticleSystem(text5);
    ParticleSystem* text6 = new ParticleSystem(ParticleSystemType::BILLBOARD,"./resources/cave/TEXT/text0.png");
    text6->isText = true;
    text6->SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
    renderkernel.AddParticleSystem(text6);
    ParticleSystem* text7 = new ParticleSystem(ParticleSystemType::BILLBOARD,"./resources/cave/TEXT/final.png");
    text7->isText = true;
    text7->SetPosition(glm::vec3(0.0f, 3.4f, 0.0f));
    renderkernel.AddParticleSystem(text7);
    
    //FOG
    /**/
    ParticleSystem* fog = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/fog.png", 2, 2);
    fog->SetPosition(glm::vec3(0.350f, -0.180f, 19.440f));
    fog->SetRandomRotation(360.f);
    fog->SetScale(5.0f);
    fog->SetProperties(12.f, 5.0f, 0.0f, 0.0f, 30.0f, 0.0f);
    fog->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(1.0f,1.0f,1.0f,0.0f), true, true));
    fog->AddColorLife(ParticleLifeColor(50.0f, glm::vec4(1.f, 1.f, 1.f, 0.10f), true, true));
    fog->AddColorLife(ParticleLifeColor(100.f, glm::vec4(1.f, 1.f, 1.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fog);
    /**/
    //ANIMATION MODEL
    //AnimatedModel* animModel = new AnimatedModel("./resources/cave/ANIMATION_MODELS/model.dae");
    //Animation* Anim_Test_Walk = new Animation("Walk", glm::vec2(0, 0.8), 2);
    //animModel->AddAnimation(Anim_Test_Walk);
    //renderkernel.AddAnimatedModel(animModel);

    //ARMS
    /**/
    Model* arms = new Model("./resources/cave/ANIMATION_MODELS/arm/arms.obj");
    arms->matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::FPS);
    arms->SetDemoType(DEMO::IS_ARM);
    arms->transform->model = glm::rotate(arms->transform->model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    arms->transform->model = glm::rotate(arms->transform->model, glm::radians(-25.0f), glm::vec3(1.0, 0.0, 0.0));
    arms->transform->model = glm::translate(arms->transform->model, glm::vec3(0.0f, -0.3f, 0.2f));
    renderkernel.AddModel(arms);
    
    Model* armsWeapon = new Model("./resources/3DModels/shotgun/Quad_Shotgun.obj");
    armsWeapon->SetDemoType(DEMO::IS_WEAPON);
    armsWeapon->matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::FPS);
    armsWeapon->transform->model = glm::rotate(armsWeapon->transform->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    armsWeapon->transform->model = glm::translate(armsWeapon->transform->model, glm::vec3(0.3f, -0.2f, 0.3f));
    renderkernel.AddModel(armsWeapon);

    Model* weapon = new Model("./resources/3DModels/shotgun/Quad_Shotgun.obj");
    weapon->SetDemoType(DEMO::IS_APPEARED);
    weapon->transform->model = glm::translate(weapon->transform->model, glm::vec3(-7.21f, 0.5f, 44.53f));
    renderkernel.AddModel(weapon);
    /**/
    //ARTIFACT
    Model* artifact = new Model("./resources/cave/ARTIFACT/artifact.obj");
    artifact->SetDemoType(DEMO::IS_ARTIFACT);
    artifact->transform->model = glm::translate(artifact->transform->model, glm::vec3(-25.250f, -1.840, 42.910));
    renderkernel.AddModel(artifact);

    //Brazier & Fire
    /**/
    Model brazier("./resources/cave/BRAZIERS/braziers.obj");
    brazier.ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(&brazier);
    
    ParticleSystem* fireBrazier1 = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/Sparks.png", 1, 1);
    fireBrazier1->SetPosition(glm::vec3(0.350f, -0.180f, 19.440f));
    fireBrazier1->SetScale(0.2f);
    fireBrazier1->SetProperties(12.f, 1.448f, 2.f, -0.04f, 0.0f, 0.434f);
    fireBrazier1->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.0f), true, true));
    fireBrazier1->AddColorLife(ParticleLifeColor(13.5f, glm::vec4(1.f, 0.766f, 0.125f, 0.0f), true, false));
    fireBrazier1->AddColorLife(ParticleLifeColor(22.6f, glm::vec4(0.f, 0.f, 0.f, 1.0f), false, true));
    fireBrazier1->AddColorLife(ParticleLifeColor(31.2f, glm::vec4(1.f, 0.3661f, 0.0808f, 0.0f), true, false));
    fireBrazier1->AddColorLife(ParticleLifeColor(100.f, glm::vec4(1.f, 0.103f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazier1);

    ParticleSystem* fireBrazierGlow = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/Sparks.png", 1, 1);
    fireBrazierGlow->SetPosition(glm::vec3(0.350f, -0.180f, 19.440f));
    fireBrazierGlow->SetScale(3.306f);
    fireBrazierGlow->SetProperties(16.f, 0.248f, 2.f, -0.04f, 0.0f, 0.699f);
    fireBrazierGlow->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.0f), true, true));
    fireBrazierGlow->AddColorLife(ParticleLifeColor(13.5f, glm::vec4(1.f, 0.766f, 0.125f, 0.0f), true, false));
    fireBrazierGlow->AddColorLife(ParticleLifeColor(22.6f, glm::vec4(0.f, 0.f, 0.f, 1.0f), false, true));
    fireBrazierGlow->AddColorLife(ParticleLifeColor(31.2f, glm::vec4(1.f, 0.3661f, 0.0808f, 0.0f), true, false));
    fireBrazierGlow->AddColorLife(ParticleLifeColor(100.f, glm::vec4(1.f, 0.103f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazierGlow);

    ParticleSystem* fireBrazierFlame = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/flame.png", 2, 2);
    fireBrazierFlame->SetPosition(glm::vec3(0.350f, -0.180f, 19.440f));
    fireBrazierFlame->SetScale(1.240f);
    fireBrazierFlame->SetProperties(16.f, 0.744f, 2.f, -0.04f, 0.0f, 0.223f);
    fireBrazierFlame->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.0f), true, true));
    fireBrazierFlame->AddColorLife(ParticleLifeColor(13.5f, glm::vec4(1.f, 0.766f, 0.125f, 0.0f), true, false));
    fireBrazierFlame->AddColorLife(ParticleLifeColor(22.6f, glm::vec4(0.f, 0.f, 0.f, 1.0f), false, true));
    fireBrazierFlame->AddColorLife(ParticleLifeColor(31.2f, glm::vec4(1.f, 0.3661f, 0.0808f, 0.0f), true, false));
    fireBrazierFlame->AddColorLife(ParticleLifeColor(100.f, glm::vec4(1.f, 0.103f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazierFlame);

    ParticleSystem* fireBrazierBigFlame = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/flame.png", 2, 2);
    fireBrazierBigFlame->SetPosition(glm::vec3(0.350f, -0.180f, 19.440f));
    fireBrazierBigFlame->SetScale(1.240f);
    fireBrazierBigFlame->SetProperties(28.f, 0.992f, 2.f, -0.04f, 0.008f, 0.831f);
    fireBrazierBigFlame->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.4117f, 0.4117f, 0.4117f, 0.0f), true, true));
    fireBrazierBigFlame->AddColorLife(ParticleLifeColor(14.1f, glm::vec4(1.f, 0.8482f, 0.0f, 0.0f), true, false));
    fireBrazierBigFlame->AddColorLife(ParticleLifeColor(15.1f, glm::vec4(1.f, 0.8482f, 0.0f, 1.0f), false, true));
    fireBrazierBigFlame->AddColorLife(ParticleLifeColor(30.0f, glm::vec4(1.f, 0.4472f, 0.1985f, 0.0f), true, false));
    fireBrazierBigFlame->AddColorLife(ParticleLifeColor(100.f, glm::vec4(0.2205f, 0.0f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazierBigFlame);
    
    ParticleSystem* fireBrazier2 = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/Sparks.png", 1, 1);
    fireBrazier2->SetPosition(glm::vec3(7.340f, -0.300f, 16.670f));
    fireBrazier2->SetScale(0.2f);
    fireBrazier2->SetProperties(12.f, 1.448f, 2.f, -0.04f, 0.0f, 0.434f);
    fireBrazier2->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.0f), true, true));
    fireBrazier2->AddColorLife(ParticleLifeColor(13.5f, glm::vec4(1.f, 0.766f, 0.125f, 0.0f), true, false));
    fireBrazier2->AddColorLife(ParticleLifeColor(22.6f, glm::vec4(0.f, 0.f, 0.f, 1.0f), false, true));
    fireBrazier2->AddColorLife(ParticleLifeColor(31.2f, glm::vec4(1.f, 0.3661f, 0.0808f, 0.0f), true, false));
    fireBrazier2->AddColorLife(ParticleLifeColor(100.f, glm::vec4(1.f, 0.103f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazier2);

    ParticleSystem* fireBrazierGlow2 = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/Sparks.png", 1, 1);
    fireBrazierGlow2->SetPosition(glm::vec3(7.340f, -0.300f, 16.670f));
    fireBrazierGlow2->SetScale(3.306f);
    fireBrazierGlow2->SetProperties(16.f, 0.248f, 2.f, -0.04f, 0.0f, 0.699f);
    fireBrazierGlow2->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.0f), true, true));
    fireBrazierGlow2->AddColorLife(ParticleLifeColor(13.5f, glm::vec4(1.f, 0.766f, 0.125f, 0.0f), true, false));
    fireBrazierGlow2->AddColorLife(ParticleLifeColor(22.6f, glm::vec4(0.f, 0.f, 0.f, 1.0f), false, true));
    fireBrazierGlow2->AddColorLife(ParticleLifeColor(31.2f, glm::vec4(1.f, 0.3661f, 0.0808f, 0.0f), true, false));
    fireBrazierGlow2->AddColorLife(ParticleLifeColor(100.f, glm::vec4(1.f, 0.103f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazierGlow2);

    ParticleSystem* fireBrazierFlame2 = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/flame.png", 2, 2);
    fireBrazierFlame2->SetPosition(glm::vec3(7.340f, -0.300f, 16.670f));
    fireBrazierFlame2->SetScale(1.240f);
    fireBrazierFlame2->SetProperties(16.f, 0.744f, 2.f, -0.04f, 0.0f, 0.223f);
    fireBrazierFlame2->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.0f), true, true));
    fireBrazierFlame2->AddColorLife(ParticleLifeColor(13.5f, glm::vec4(1.f, 0.766f, 0.125f, 0.0f), true, false));
    fireBrazierFlame2->AddColorLife(ParticleLifeColor(22.6f, glm::vec4(0.f, 0.f, 0.f, 1.0f), false, true));
    fireBrazierFlame2->AddColorLife(ParticleLifeColor(31.2f, glm::vec4(1.f, 0.3661f, 0.0808f, 0.0f), true, false));
    fireBrazierFlame2->AddColorLife(ParticleLifeColor(100.f, glm::vec4(1.f, 0.103f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazierFlame2);

    ParticleSystem* fireBrazierBigFlame2 = new ParticleSystem(ParticleSystemType::COMMON_PS, "./resources/cave/PARTICLE_SYSTEMS/flame.png", 2, 2);
    fireBrazierBigFlame2->SetPosition(glm::vec3(7.340f, -0.300f, 16.670f));
    fireBrazierBigFlame2->SetScale(1.240f);
    fireBrazierBigFlame2->SetProperties(28.f, 0.992f, 2.f, -0.04f, 0.008f, 0.831f);
    fireBrazierBigFlame2->AddColorLife(ParticleLifeColor(0.0f, glm::vec4(0.4117f, 0.4117f, 0.4117f, 0.0f), true, true));
    fireBrazierBigFlame2->AddColorLife(ParticleLifeColor(14.1f, glm::vec4(1.f, 0.8482f, 0.0f, 0.0f), true, false));
    fireBrazierBigFlame2->AddColorLife(ParticleLifeColor(15.1f, glm::vec4(1.f, 0.8482f, 0.0f, 1.0f), false, true));
    fireBrazierBigFlame2->AddColorLife(ParticleLifeColor(30.0f, glm::vec4(1.f, 0.4472f, 0.1985f, 0.0f), true, false));
    fireBrazierBigFlame2->AddColorLife(ParticleLifeColor(100.f, glm::vec4(0.2205f, 0.0f, 0.0f, 0.0f), true, true));
    renderkernel.AddParticleSystem(fireBrazierBigFlame2);
    /**/
    //Nature forest
    /**/
    Model aspen("./resources/cave/NATURE/aspen.obj");
    aspen.ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(&aspen);
    Model pines("./resources/cave/NATURE/pines.obj");
    pines.ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(&pines);
    
    //Sculpture
    Model *sculpture_magic = new Model("./resources/cave/MAGIC_SCULPTURE/magic_sculpture.obj");
    sculpture_magic->RotationTo(-90.0f, glm::vec3(0.0, 1.0, 0.0));
    sculpture_magic->transform->model = glm::translate(sculpture_magic->transform->model, glm::vec3(0.0, 2.0, 0.0));
    sculpture_magic->ScaleTo(glm::vec3(0.005f));
    sculpture_magic->SetDemoType(DEMO::IS_STATUE);
    renderkernel.AddModel(sculpture_magic);
    /**/
    //Fallen lord
    Model *fallenLord = new Model("./resources/cave/LORDFALLEN/lordFallen.obj");
    fallenLord->ScaleTo(glm::vec3(0.005f));
    renderkernel.AddModel(fallenLord);

    //MAGIC BASE
    GodRay* magicBase = new GodRay("./resources/cave/MAGIC_BASE/green.obj");
    magicBase->ScaleTo(glm::vec3(0.5f));
    magicBase->transform->model = glm::translate(magicBase->transform->model, glm::vec3(0.0f, -0.2f, 0.0f));
    renderkernel.AddGodRayModel(magicBase);

    GodRay* magicBaseG = new GodRay("./resources/cave/MAGIC_BASE/redBase.obj");
    magicBaseG->transform->model = glm::translate(magicBaseG->transform->model, glm::vec3(-7.21f, -2.3f, 44.53f));
    magicBaseG->ScaleTo(glm::vec3(0.5f));
    renderkernel.AddGodRayModel(magicBaseG);
    magicBaseG->isActive = false;
    demo->magicBase = magicBaseG;

    //RAY MARCHING TECHNIQUE
    //Shader* rmShader = new Shader("shaders/rmclouds.vert", "shaders/rmclouds.frag");
    //std::vector<Texture> noiseTextures;
    //noiseTextures.push_back(Texture("resources/noise/noiseFBM.png", TypeTexture::NOISE));
    //Material* rmMaterial = new Material(rmShader, noiseTextures);

    ///--CUBE   
    //Model cubeModel(vertices, 36, textures_pbr);
    //cubeModel.SetPositionOrientation(reactphysics3d::Vector3(0.0f, 30.0f, 0.0f));
    //cubeModel.SetPhysicType(PhysicBodyType::RIGIDBODY_DINAMIC);
    //cubeModel.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(0.25f));
    //renderkernel.AddModel(&cubeModel);

    //Model cubeModel2(vertices, 36, textures_pbr);
    //cubeModel2.SetPositionOrientation(reactphysics3d::Vector3(-5.0f, 0.0f, 0.0f));
    //cubeModel2.SetPhysicType(PhysicBodyType::RIGIDBODY_STATIC);
    //cubeModel2.SetCollider(ColliderShapeType::BOX_SHAPE, glm::vec3(30.f, 0.1f, 30.f));
    //renderkernel.AddModel(&cubeModel2);

    //Model cubeModel3(vertices, 36, textures_pbr);
    //cubeModel3.SetPositionOrientation(reactphysics3d::Vector3(0.0f, 0.0f, 0.0f));
    //cubeModel3.SetPhysicType(PhysicBodyType::RIGIDBODY_STATIC);
    //cubeModel3.SetCollider(ColliderShapeType::MESH_SHAPE);
    //renderkernel.AddModel(&cubeModel3);

   // cubeModel.isSelectableModel = true;
    //cubeModel.AddMaterial(rmMaterial);
   // cubeModel.ScaleTo(glm::vec3(4.0f, 4.0f, 4.0f));
    //cubeModel.TranslationTo(glm::vec3(0.0f, 1.1f, 0.0f));

    //cubeModel.matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::OUTLINE);
    //renderkernel.AddModel(&cubeModel);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::BLINN, false);
    //cubeModel.isSelectable(true);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::SHININESS, 8.0f);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::A_REFRACTIVE, true);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::REFRACTIVE_INDEX, 1.31f);
    //Model cubeModel2(vertices, 36, textures_pbr);
    //cubeModel2.TranslationTo(glm::vec3(0.0f, 0.0f, 0.0f));
    //cubeModel2.ScaleTo(glm::vec3(0.5f));
    //renderkernel.AddModel(&cubeModel2);

    //cubeModel.AttachModel(&cubeModel2);

    //Model cubeModel3(vertices, 36, textures);
    //cubeModel3.AddMaterial(emissiveMaterial);
    //cubeModel3.TranslationTo(glm::vec3(0.0f, 0.0f, 0.0f));
    //cubeModel3.Rotation(60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
    //cubeModel3.ScaleTo(glm::vec3(0.25f));
    //renderkernel.AddModel(&cubeModel3);

    ///--FLOOR
    //Model floorModel(floorVertices, 6, textures_parallax);
    //floorModel.isSelectableModel = true;
    //floorModel.matHandle.EditMaterial(MaterialComponent::SHININESS, 32.0f);
    //floorModel.matHandle.EditMaterial(MaterialComponent::BLOOM_BRIGHTNESS, 100.0f);
    //floorModel.matHandle.EditMaterial(MaterialComponent::P_DISPLACEMENT, -0.1f);
    //floorModel.matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::OUTLINE);
    //floorModel.matHandle.EditMaterial(MaterialComponent::MIN_UV, 0.0f);
    //floorModel.matHandle.EditMaterial(MaterialComponent::MAX_UV, 5.0f); 
    //Shader* sh = new Shader("shaders/parallaxMapping.vert", "shaders/parallaxMapping.frag");
    //Material* mat = new Material(sh);
    //mat->AddMultTextures(textures_parallax);
    //floorModel.AddMaterial(mat);
    //floorModel.matHandle.EditMaterial(MaterialComponent::P_DISPLACEMENT, 0.05f);
    //floorModel.matHandle.EditMaterial(MaterialComponent::SHININESS, 64.0f);
    //floorModel.matHandle.EditMaterial(MaterialComponent::BLINN, true);
    //floorModel.matHandle.EditMaterial(MaterialComponent::SHADER1, renderShader);
    //floorModel.ScaleTo(glm::vec3(1.0, 1.0, 1.0));
    //floorModel.transform->model = glm::rotate(floorModel.transform->model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
    //floorModel.transform->model = glm::translate(floorModel.transform->model, glm::vec3(0.0, -9.0, 0.0));
    //renderkernel.AddModel(&floorModel);

    //Model floorModel2(floorVertices, 6, textures_parallax);
    //floorModel2.matHandle.EditMaterial(MaterialComponent::P_DISPLACEMENT, -0.1f);
    //floorModel2.ScaleTo(glm::vec3(1.0, 1.0, 1.0));
    //floorModel2.transform->model = glm::rotate(floorModel2.transform->model, glm::radians(-90.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
    //floorModel2.TranslationTo(glm::vec3(1.0, 1.0, 0.0));
    //renderkernel.AddModel(&floorModel2);

    ///--VEGETATION
    /*
    Model vegetation(transparentVertices, 6, 8);
    vegetation.TranslationTo(glm::vec3(1.0f, -1.0f, 0.0f));
    vegetation.AddMaterial(transpVegi);
    //renderkernel.AddModel(&vegetation);
    */
    ///--GEOMETRY
    /*
    //Model geometryModel(points, 4, 5, true);
    //geometryModel.AddMaterial(geoMaterial);
    //geometryModel.matHandle.EditMaterial(MaterialComponent::DRAW_MODE, DrawMode::DPOINTS);
    //renderkernel.AddModel(&geometryModel);
    */
    ///--INSTANCES
    /*
    // generate a list of 100 quad locations/translation-vectors
    // ---------------------------------------------------------
    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    Model instanceModel(quadVertices, 6, 5, ModelType::INSTANCE_M, translations);
    instanceModel.AddMaterial(instanceMaterial);
    instanceModel.matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INSTANCE);
    renderkernel.AddModel(&instanceModel);
    */

    ///--Planet
    //Model planet("./resources/3DModels/planet/planet.obj");
    //planet.ScaleTo(glm::vec3(10.0f));
    //planet.matHandle.EditMaterial(MaterialComponent::SHININESS, 12.0f);
    //renderkernel.AddModel(&planet);
    ///--Asteroid
    //Model asteroid("./resources/3DModels/asteroid/rock.obj");
    //asteroid.AddMaterial(mat_asteroid);
    //asteroid.matHandle.EditMaterial(MaterialComponent::TYPE, MaterialType::INSTANCE, 1000); 
    //asteroid.matHandle.EditMaterial(MaterialComponent::SHININESS, 12.0f);
    //renderkernel.AddModel(&asteroid);

    // VOLUME RENDER

    //RenderVolume* rdVolume = new RenderVolume(RenderVolumeType::BOX_VOL, NoiseType::WORLEY);
    //renderkernel.AddRenderVolume(rdVolume);

    // LIGHT
    
    Light* pointLight = new Light(TypeLight::POINTLIGHT, glm::vec3(1.86f, 5.0f, 0.0f));
    pointLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(5.0f, 15.0f, 5.0f));
    pointLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(15.0f, 15.0f, 15.0f));
    pointLight->EditLightComponent(LightComponent::LIGHT_LINEAR, 20.0f);
    pointLight->EditLightComponent(LightComponent::LIGHT_FAR_EFFECT, 60.0f);
    pointLight->SetFarPlane(60.0f);
    //pointLight->GetFarplane() = 60.0f;
    //pointLight->
    renderkernel.AddLight(pointLight);

    Light* weaponLight = new Light(TypeLight::POINTLIGHT, glm::vec3(-7.f, -0.64f, 44.5f));//WEAPON LIGHT
    weaponLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(0.0f));
    weaponLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(0.0f));
    //weaponLight->EditLightComponent(LightComponent::LIGHT_LINEAR, 0.0f);
    weaponLight->EditLightComponent(LightComponent::LIGHT_FAR_EFFECT, 40.0f);
    //weaponLight->SetLinear(0.0f);
    renderkernel.AddLight(weaponLight);
    demo->AddWeaponLight(weaponLight);

    Light* artifactLight = new Light(TypeLight::POINTLIGHT, glm::vec3(-25.250f, -1.840, 42.910));//ARTIFACT LIGHT
    artifactLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(15.0f, 5.0f, 15.0f));
    artifactLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(15.0f, 5.0f, 15.0f));
    artifactLight->EditLightComponent(LightComponent::LIGHT_LINEAR, 10.0f); 
    artifactLight->SetLinear(0.0f);
    renderkernel.AddLight(artifactLight);
    artifactLight->isCastShadow = false;
    demo->AddArtifactLight(artifactLight);
    
    //Light* spotLight = new Light(TypeLight::SPOTL, glm::vec3(5.0f, 0.5f, 0.0f));
    //spotLight->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(-1.0, 0.0, 0.0));
    //spotLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(15.0f, 15.0f, 15.0f)); //glm::vec3(4.6, 20.0, 1.6));
    //spotLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(15.0f, 15.0f, 15.0f)); //glm::vec3(0.23, 1.0, 0.08));
    //spotLight->EditLightComponent(LightComponent::LIGHT_LINEAR, 20.0f);
    //spotLight->EditLightComponent(LightComponent::LIGHT_CUTOFF, 25.0f);
    //spotLight->EditLightComponent(LightComponent::LIGHT_OUTERCUTOFF, 30.0f);
    //renderkernel.AddLight(spotLight);

    Light* dirLight = new Light(TypeLight::DIRL, glm::vec3(2.23f, 9.39f, 10.62f));
    dirLight->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(-8.24f, 1.16f, -0.05));
    dirLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(6.f, 1.6f, 1.6f)); 
    dirLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(15.0f, 10.0f, 10.0f));
    renderkernel.AddLight(dirLight);

    Texture* textureHDR = new Texture("resources/HDR/Desert_Highway/101_hdrmaps_com_free.hdr",TypeTexture::HDR_SKYBOX);
    Skybox* skybox = new Skybox(true);
    skybox->SetHdrTexture(textureHDR);
    renderkernel.AddPreCookSkybox(skybox);

    ///std::vector<Texture> texPBR;
    ///texPBR.push_back(*textureHDR);
    ///Model cubeModel3(vertices, 36, texPBR);
    ///Shader* shr = new Shader("shaders/sphericalCubemapTest.vert", "shaders/sphericalCubemapTest.frag");
    ///cubeModel3.matHandle.forward = shr;// EditMaterial(MaterialComponent::SHADER1, shr);
    ///cubeModel3.matHandle.shader = shr;
    ///cubeModel3.matHandle.EditMaterial(MaterialComponent::TEXTURE, texPBR);
    ///cubeModel3.TranslationTo(glm::vec3(0.0f, 3.0f, 0.0f));
    ///renderkernel.AddModel(&cubeModel3);

    //GLint* max = new GLint(32);
    //glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, max);

    //FBO
    renderkernel.PreRender();

    // Main loop
    renderkernel.StartRender();

    first.join();
    return 0;
}


