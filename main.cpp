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
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
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

    ImVec4* clear_color = new ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    //ImVec4* clear_color = new ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

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
    Texture textureFloor("./resources/wood.png", TypeTexture::DIFFUSE, true);
    std::vector<Texture> textures_floor;
    textures_floor.push_back(textureFloor);

    Texture parallaxDiff("./resources/bricks/bricks2.jpg", TypeTexture::DIFFUSE);
    Texture parallaxNorm("./resources/bricks/bricks2_normal.jpg", TypeTexture::NORMAL);
    Texture parallaxDisp("./resources/bricks/bricks2_disp.jpg", TypeTexture::HEIGHT);
    std::vector<Texture> textures_parallax;
    textures_parallax.push_back(parallaxDiff);
    textures_parallax.push_back(parallaxNorm);
    textures_parallax.push_back(parallaxDisp);

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
    
    ///CAMERA
    Camera mainCamera(1280, 720);
    renderkernel.AddCamera(&mainCamera);

    ///MODEL 3D
    ///--NANOSUIT  
    //Model ourModel("./resources/3DModels/crysis/nanosuit.obj");
    Model ourModel("./resources/3DModels/darksouls/fallen.obj"); 
    //ourModel.matHandle.EditMaterial(MaterialComponent::SHADER1, renderShader);
    /*ourModel.AddMaterial(normalMaterial);
    //ourModel.matHandle.EditMaterial(MaterialComponent::A_REFRACTIVE, true);
    //ourModel.matHandle.EditMaterial(MaterialComponent::REFRACTIVE_INDEX, 1.52f);
    */
    //ourModel.ScaleTo(glm::vec3(0.1f));
    ourModel.TranslationTo(glm::vec3(0.0f, -0.5f, 0.0f));
    renderkernel.AddModel(&ourModel);
    
    ///--CUBE   
    //Model cubeModel(vertices, 36, textures);
    //cubeModel.TranslationTo(glm::vec3(0.0f, 0.0f, 0.0f));
    //cubeModel.ScaleTo(glm::vec3(0.5f));
    //renderkernel.AddModel(&cubeModel);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::BLINN, false);
    //cubeModel.isSelectable(true);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::SHININESS, 8.0f);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::A_REFRACTIVE, true);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::REFRACTIVE_INDEX, 1.31f);
    //Model cubeModel2(vertices, 36, textures);
    //cubeModel2.TranslationTo(glm::vec3(2.0f, -0.5f, 1.0f));
    //cubeModel2.ScaleTo(glm::vec3(0.5f));
    //renderkernel.AddModel(&cubeModel2);
    //Model cubeModel3(vertices, 36, textures);
    //cubeModel3.TranslationTo(glm::vec3(-1.0f, 0.0f, 2.0f));
    //cubeModel3.Rotation(60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
    //cubeModel3.ScaleTo(glm::vec3(0.25f));
    //renderkernel.AddModel(&cubeModel3);

    ///--FLOOR
    Model floorModel(floorVertices, 6, textures_floor);
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
    
    Light* dirLight = new Light(TypeLight::DIRL, glm::vec3(-5.0f, 7.0f, -5.0f));
    dirLight->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(0.5, -1.0, 0.5));
    dirLight->EditLightComponent(LightComponent::LIGHT_DIFFUSE, glm::vec3(1.0, 0.1, 0.1));
    dirLight->EditLightComponent(LightComponent::LIGHT_SPECULAR, glm::vec3(1.0, 0.1, 0.1));
    renderkernel.AddLight(dirLight);
    /*
    Light* dirLight2 = new Light(TypeLight::DIRL, glm::vec3(5.0f, 7.0f, -5.0f)); 
    dirLight2->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(-0.5, -1.0, 0.5));
    renderkernel.AddLight(dirLight2);
    Light* dirLight3 = new Light(TypeLight::DIRL, glm::vec3(-5.0f, 7.0f, 5.0f));
    dirLight3->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(0.5, -1.0, -0.5));
    renderkernel.AddLight(dirLight3);
    Light* dirLight4 = new Light(TypeLight::DIRL, glm::vec3(5.0f, 7.0f, 5.0f)); 
    dirLight4->EditLightComponent(LightComponent::LIGHT_DIRECTION, glm::vec3(-0.5, -1.0, -0.5));
    renderkernel.AddLight(dirLight4);
    */
    Light* pointLight = new Light(TypeLight::POINTLIGHT, glm::vec3(-5.0f, 7.0f, -5.0f)); 
    renderkernel.AddLight(pointLight);
    Light* pointLight2 = new Light(TypeLight::POINTLIGHT, glm::vec3(5.0f, 7.0f, -5.0f));
    renderkernel.AddLight(pointLight2);
    Light* pointLight3 = new Light(TypeLight::POINTLIGHT, glm::vec3(-5.0f, 7.0f, 5.0f));
    renderkernel.AddLight(pointLight3);
    Light* pointLight4 = new Light(TypeLight::POINTLIGHT, glm::vec3(5.0f, 7.0f, 5.0f));
    renderkernel.AddLight(pointLight4);

    //renderkernel.AddLight(new Light(TypeLight::SPOTFPSL));
    //renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[0]));
    //renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[1]));
    //renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[2]));
    //renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[3]));

    //renderkernel.lights.at(0)->SetSpecular(glm::vec3(1.25f));
    //renderkernel.lights.at(1)->SetSpecular(glm::vec3(0.5f));
    //renderkernel.lights.at(2)->SetSpecular(glm::vec3(0.75f));
    //renderkernel.lights.at(3)->SetSpecular(glm::vec3(1.0f));
    //renderkernel.lights.at(0)->SetDiffuse(glm::vec3(1.25f));
    //renderkernel.lights.at(1)->SetDiffuse(glm::vec3(0.5f));
    //renderkernel.lights.at(2)->SetDiffuse(glm::vec3(0.75f));
    //renderkernel.lights.at(3)->SetDiffuse(glm::vec3(1.0f));

    //FBO
    renderkernel.PreRender();

    // Main loop
    renderkernel.StartRender();


    return 0;
}


