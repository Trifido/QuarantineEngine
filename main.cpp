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

    ImVec4* clear_color = new ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    //ImVec4* clear_color = new ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    //RENDER SYSTEM
    RenderSystem renderkernel(window, clear_color);

    ///TEXTURES
    std::vector<Texture> textures;
    Texture texture1("resources/brickwall.jpg", TypeTexture::DIFFUSE);
    Texture texture2("resources/brickwall_normal.jpg", TypeTexture::NORMAL);
    Texture texture3("resources/matrix.jpg", TypeTexture::EMISSIVE);
    Texture texture4("./resources/grass.png", TypeTexture::DIFFUSE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    textures.push_back(texture1);
    textures.push_back(texture2);
    //textures.push_back(texture3);

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
    Shader* standardShader = new Shader("shaders/standardLighting.vert", "shaders/standardLighting.frag");
    //Shader* instancingShader = new Shader("shaders/instancing.vert", "shaders/instancing.frag");
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
    transpVegi->shininess = 12.0f;*/

    //Material* mat_asteroid = new Material(instancingShader);
    ///CAMERA
    Camera mainCamera(1280, 720);
    renderkernel.AddCamera(&mainCamera);

    ///MODEL 3D
    ///--NANOSUIT
   
    //Model ourModel("./resources/3DModels/crysis/nanosuit.obj");
    /*ourModel.AddMaterial(normalMaterial);
    //ourModel.matHandle.EditMaterial(MaterialComponent::A_REFRACTIVE, true);
    //ourModel.matHandle.EditMaterial(MaterialComponent::REFRACTIVE_INDEX, 1.52f);
    ourModel.ScaleTo(glm::vec3(0.2f));
    ourModel.TranslationTo(glm::vec3(0.0f, -7.75f, 0.0f));*/
    //renderkernel.AddModel(&ourModel);
    
    ///--CUBE
    
    Model cubeModel(vertices, 36, textures);
    //cubeModel.isSelectable(true);
    cubeModel.matHandle.EditMaterial(MaterialComponent::SHININESS, 12.0f);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::A_REFRACTIVE, true);
    //cubeModel.matHandle.EditMaterial(MaterialComponent::REFRACTIVE_INDEX, 1.31f);
    cubeModel.TranslationTo(glm::vec3(0.0f, 0.0f, -4.0f));
    cubeModel.ScaleTo(glm::vec3(4.0f));
    renderkernel.AddModel(&cubeModel);
    
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
    renderkernel.AddLight(new Light(TypeLight::DIRL));
    renderkernel.AddLight(new Light(TypeLight::SPOTFPSL));
    renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[0]));
    renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[1]));
    renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[2]));
    renderkernel.AddLight(new Light(TypeLight::POINTLIGHT, pointLightPositions[3]));

    //FBO
    renderkernel.PreRender();

    // Main loop
    renderkernel.StartRender();


    return 0;
}


