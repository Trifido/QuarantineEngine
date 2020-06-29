#include "Requisites.h"

int main(int, char**)
{
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

    //RENDER SYSTEM
    RenderSystem renderkernel(window, clear_color);

    ///TEXTURES
    std::vector<Texture> textures;
    Texture texture1("resources/brickwall.jpg", TypeTexture::DIFFUSE, true);
    Texture texture2("resources/brickwall_normal.jpg", TypeTexture::NORMAL); 
    //Texture texture3("resources/matrix.jpg", TypeTexture::EMISSIVE);
    //Texture texture4("./resources/grass.png", TypeTexture::DIFFUSE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    textures.push_back(texture1);
    textures.push_back(texture2);
    //textures.push_back(texture3);
    //Texture textureFloor("./resources/wood.png", TypeTexture::DIFFUSE, false);
    //std::vector<Texture> textures_floor;
    //textures_floor.push_back(textureFloor);

    Texture parallaxDiff("resources/materials/floor_dirt/Stone_Path_001_baseColor.jpg", TypeTexture::DIFFUSE);
    Texture parallaxNorm("resources/materials/floor_dirt/Stone_Path_001_normal.jpg", TypeTexture::NORMAL);
    Texture parallaxDisp("resources/materials/floor_dirt/Stone_Path_001_height.png", TypeTexture::HEIGHT);
    Texture parallaxRoug("resources/materials/floor_dirt/Stone_Path_001_roughness.jpg", TypeTexture::ROUGHNESS);
    Texture parallaxAO("resources/materials/floor_dirt/Stone_Path_001_ambientOcclusion.jpg", TypeTexture::AO);

    std::vector<Texture> textures_parallax;
    textures_parallax.push_back(parallaxDiff);
    textures_parallax.push_back(parallaxNorm);
    textures_parallax.push_back(parallaxDisp);
    textures_parallax.push_back(parallaxRoug);
    textures_parallax.push_back(parallaxAO);


    std::vector<Texture> textures_pbr;
    Texture pbrDiff("resources/PBR_Textures/rustedIron/rustediron2_basecolor.png", TypeTexture::DIFFUSE);
    Texture pbrNorm("resources/PBR_Textures/rustedIron/rustediron2_normal.png", TypeTexture::NORMAL);
    Texture pbrME("resources/PBR_Textures/rustedIron/rustediron2_metallic.png", TypeTexture::METALLIC);
    Texture pbrRO("resources/PBR_Textures/rustedIron/rustediron2_roughness.png", TypeTexture::ROUGHNESS);
    Texture pbrAO("resources/PBR_Textures/rustedIron/rustediron2_roughness.png", TypeTexture::AO);

    textures_pbr.push_back(pbrDiff);
    textures_pbr.push_back(pbrNorm);
    textures_pbr.push_back(pbrME);
    textures_pbr.push_back(pbrRO);
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
    //Model ourModel("./resources/3DModels/colt/model.obj");
    Model ourModel("./resources/3DModels/fps_arms/obj/arms.obj");
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
    //ourModel.ScaleTo(glm::vec3(0.05f));
    //ourModel.Rotation(90.0f,glm::vec3(1.0, 0.0, 1.0));
    //ourModel.TranslationTo(glm::vec3(0.0f, -0.5f, 0.0f));

    //ourModel.matHandle.EditMaterial(MaterialComponent::TEXTURE, textures_pbr);
    renderkernel.AddModel(&ourModel);

    ourModel.AttachCamera(&mainCamera);
    //BoundingLight* bdLight = new BoundingLight();
    //bdLight->TranslationTo(glm::vec3(0.0f, 0.0f, 0.0f));
    //renderkernel.AddLight(bdLight);


    ///--SPHERE
    //Model sphereModel("./resources/3DModels/sphere/sphere.obj");
    //sphereModel.matHandle.EditMaterial(MaterialComponent::TEXTURE, textures_pbr);
    //sphereModel.meshes.at(0) = *(bdLight->mesh);
    //sphereModel.matHandle.listMaterials.at(0) = bdLight->matHandle.listMaterials.at(0);
    //sphereModel.TranslationTo(glm::vec3(0.0f, 4.0f, 0.0f));
    //sphereModel.matHandle.listMaterials.at(0)->isBounding = true;
    //renderkernel.AddModel(&sphereModel);

    

    ///--CUBE   
    Model cubeModel(vertices, 36, textures_pbr);
    cubeModel.TranslationTo(glm::vec3(1.0f, 0.0f, 0.0f));
    //cubeModel.ScaleTo(glm::vec3(0.5f));
    renderkernel.AddModel(&cubeModel);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::BLINN, false);
    //cubeModel.isSelectable(true);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::SHININESS, 8.0f);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::A_REFRACTIVE, true);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::REFRACTIVE_INDEX, 1.31f);
    Model cubeModel2(vertices, 36, textures_pbr);
    cubeModel2.TranslationTo(glm::vec3(0.0f, 1.0f, 0.0f));
    //cubeModel2.ScaleTo(glm::vec3(0.5f));
    renderkernel.AddModel(&cubeModel2);

    cubeModel.AttachModel(&cubeModel2);

    Model cubeModel3(vertices, 36, textures);
    //cubeModel3.AddMaterial(emissiveMaterial);
    cubeModel3.TranslationTo(glm::vec3(0.0f, 0.0f, 0.0f));
    //cubeModel3.Rotation(60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
    //cubeModel3.ScaleTo(glm::vec3(0.25f));
    //renderkernel.AddModel(&cubeModel3);

    ///--FLOOR
    Model floorModel(floorVertices, 6, textures_parallax);
    //floorModel.matHandle.EditMaterial(MaterialComponent::SHININESS, 32.0f);
    floorModel.matHandle.EditMaterial(MaterialComponent::P_DISPLACEMENT, -0.1f);
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
    renderkernel.AddModel(&floorModel);

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

    // LIGHT
    
    Light* pointLight = new Light(TypeLight::POINTLIGHT, glm::vec3(5.0f, 1.0f, 0.0f)); 
    //pointLight->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(0.01, -1.0, 0.01));
    pointLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(15.0f, 15.0f, 15.0f)); //glm::vec3(4.6, 20.0, 1.6));
    pointLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(15.0f, 15.0f, 15.0f)); //glm::vec3(0.23, 1.0, 0.08));
    pointLight->EditLightComponent(LightComponent::LIGHT_LINEAR, 10.0f); //glm::vec3(0.23, 1.0, 0.08));
    renderkernel.AddLight(pointLight);

    
    Light* dirLight2 = new Light(TypeLight::POINTLIGHT, glm::vec3(-5.0f, 1.0f, 0.0f));
    dirLight2->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(0.5, -1.0, 0.5));
    dirLight2->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(15.0f, 1.0f, 15.0f)); //glm::vec3(4.6, 20.0, 1.6));
    dirLight2->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(15.0f, 15.0f, 15.0f)); //glm::vec3(0.23, 1.0, 0.08));
    //renderkernel.AddLight(dirLight2);


    Texture* textureHDR = new Texture("resources/HDR/Desert_Highway/Road_to_MonumentValley_Ref.hdr",TypeTexture::HDR_SKYBOX);
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


    return 0;
}


