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
    textures.push_back(texture3);

    ///SHADERS
    Shader* sh2 = new Shader("shaders/nanosuit.vert", "shaders/nanosuit.frag");
    Shader* outlineShader = new Shader("shaders/outline.vert", "shaders/outline.frag");
    Shader* transparentShader = new Shader("shaders/blending.vert", "shaders/blending.frag");
    Shader* reflectiveShader = new Shader("shaders/reflective.vert", "shaders/reflective.frag");
    Shader* reflectiveShader2 = new Shader("shaders/reflective.vert", "shaders/reflective.frag");
    

    ///MATERIALES
    Material *cubeMaterial = new Material(reflectiveShader, outlineShader, textures);
    Material *nanosuitMaterial = new Material(reflectiveShader2);
    cubeMaterial->shininess = 12.0f;

    Material* transpVegi = new Material(transparentShader);
    textures.clear();
    textures.push_back(texture4);
    transpVegi->AddMultTextures(textures);
    transpVegi->type = MaterialType::TRANSP;
    transpVegi->shininess = 12.0f;

    ///CAMERA
    Camera mainCamera(1280, 720);
    renderkernel.AddCamera(&mainCamera);

    ///MODEL 3D
    ///--NANOSUIT
    Model ourModel("./resources/3DModels/crysis/nanosuit.obj");
    ourModel.AddMaterial(nanosuitMaterial);
    ourModel.matHandle.EditMaterial(MaterialComponent::AREFLECTIVE, true);
    ourModel.ScaleTo(glm::vec3(0.2f));
    ourModel.TranslationTo(glm::vec3(0.0f, -7.75f, 0.0f));
    renderkernel.AddModel(&ourModel);

    ///--CUBE
    Model cubeModel(vertices, 36, textures);
    cubeModel.isSelectable(true);
    cubeModel.AddMaterial(cubeMaterial);
    cubeModel.matHandle.EditMaterial(MaterialComponent::AREFLECTIVE, true);
    cubeModel.TranslationTo(glm::vec3(0.0f, 0.0f, -4.0f));
    cubeModel.ScaleTo(glm::vec3(4.0f));
    renderkernel.AddModel(&cubeModel);

    ///--VEGETATION
    Model vegetation(transparentVertices, 6);
    vegetation.TranslationTo(glm::vec3(1.0f, -1.0f, 0.0f));
    vegetation.AddMaterial(transpVegi);
    //renderkernel.AddModel(&vegetation);

    // LIGHT 
    renderkernel.AddLight(new Light(TypeLight::DIRL));
    //renderkernel.AddLight(new Light(TypeLight::SPOTFPSL));
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


