// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream> 
#include <map>

//#define STB_IMAGE_IMPLEMENTATION
//#include "libs/stb_image.h"

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//Own functions
#include "RenderSystem.h"
#include "KeyInput.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "RawGeometry.h"
#include "RenderPlane.h"
#include "FBOSystem.h"



static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    //ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); TERROR
    //ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 

    //CAMERA
    Camera mainCamera(1280, 720);

    //POST-PROCESS RENDER PLANE
    RenderPlane renderFinalPass;
    renderFinalPass.SetVAORenderPlane();

    //MODEL 3D
    std::vector<Model*> sceneModels;
    Model ourModel("./resources/3DModels/crysis/nanosuit.obj");
    ourModel.isSelectable(true);
    //Model headModel("./resources/3DModels/sponza/sponza.obj");

    //Material shininess
    ourModel.SetShininess(1.0f);
    //headModel.SetShininess(1.0f);

    //VEGETACION
    std::vector<Texture> texturesVegetation;
    Texture diffVeg("./resources/grass.png", TypeTexture::DIFFUSE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texturesVegetation.push_back(diffVeg);
    Model vegetationM(transparentVertices, 6, texturesVegetation);
    Model plane(planeVertices, 6, texturesVegetation);
    plane.SetShininess(1.0f);
    vegetationM.isSelectable(true);

    std::vector<glm::vec3> vegetation;
    vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    
    // LIGHT
    std::vector<Light*> lightsVector;
    //lightsVector.push_back(new Light(TypeLight::SPOTFPSL));
    lightsVector.push_back(new Light(TypeLight::DIRL));
    lightsVector.push_back(new Light(TypeLight::POINTLIGHT, pointLightPositions[0]));
    lightsVector.push_back(new Light(TypeLight::POINTLIGHT, pointLightPositions[1]));
    lightsVector.push_back(new Light(TypeLight::POINTLIGHT, pointLightPositions[2]));
    lightsVector.push_back(new Light(TypeLight::POINTLIGHT, pointLightPositions[3]));

    //SHADER
    Shader nanoSuitShader("shaders/nanosuit.vert", "shaders/nanosuit.frag");
    Shader outlineShader("shaders/outline.vert", "shaders/outline.frag");
    Shader transparentShader("shaders/blending.vert", "shaders/blending.frag");
    // - Añado las propiedas a mi shader
    nanoSuitShader.AddLight(lightsVector);
    nanoSuitShader.AddCamera(&mainCamera);
    outlineShader.AddCamera(&mainCamera);
    transparentShader.AddCamera(&mainCamera);

    Texture texture1("resources/brickwall.jpg", TypeTexture::DIFFUSE); 
    Texture texture2("resources/brickwall_normal.jpg", TypeTexture::NORMAL);
    Texture texture3("resources/matrix.jpg", TypeTexture::EMISSIVE);

    std::vector<Texture> textures;
     
    textures.push_back(texture1);
    textures.push_back(texture2);
    textures.push_back(texture3);

    Model cubeModel(vertices, 36, textures);
    //Material shininess
    cubeModel.SetShininess(12.0f);
    cubeModel.isSelectable(true);

    sceneModels.push_back(&ourModel);
    sceneModels.push_back(&cubeModel);
    sceneModels.push_back(&vegetationM);

    //CREAMOS UN FRAME BUFFER OBJECT FBO
    int width, height;
    int lastWidth, lastHeight;

    glfwGetWindowSize(window, &width, &height);
    FBOSystem fboSystem(&width, &height);

    lastWidth = width;
    lastHeight = height;

    fboSystem.InitFBOSystem();

    //ACTIVAMOS EL DEPTH BUFFER
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //ACTIVAMOS EL CULLING
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    //ACTIVAMOS EL BLENDING
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Main loop
    while (!glfwWindowShouldClose(window))
    { 
        //ANIMATION
        RenderSystem::ComputeDeltaTime();
        RenderSystem::GetWindowSize(window, &width, &height);

        //Keyboad functions
        processInput(window);
        //Mouse functions
        processMouseInput(window, &mainCamera, sceneModels);

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Camera Input
        mainCamera.CameraController();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            /*
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
            */
        }
        // 3. Show another simple window.
        //if (show_another_window)
        //{
        //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //    ImGui::Text("Hello from another window!");
        //    if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //    ImGui::End();
        //}

        if (lastWidth != width || lastHeight != height)
        {
            fboSystem.InitFBOSystem();
        }

        lastWidth = width;
        lastHeight = height;
            
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        // FIRST PASS
        fboSystem.ActivateFBORender();

        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //SHADER ACTIVATE PROPERTIES
       
        transparentShader.use();
        transparentShader.ActivateCamera();

        nanoSuitShader.use();
        nanoSuitShader.ActivateCamera();
        nanoSuitShader.ActivateLights();

        outlineShader.use();
        outlineShader.ActivateCamera();
         

        //CUBE
        cubeModel.model = glm::mat4(1.0f); 
        cubeModel.model = glm::translate(cubeModel.model, glm::vec3(0.0f, 0.0f, -4.0f));
        cubeModel.model = glm::scale(cubeModel.model, glm::vec3(4.0f, 4.0f, 4.0f));	// it's a bit too big for our scene, so scale it down
        cubeModel.Draw(nanoSuitShader, outlineShader);

        //NANOSUIT
        
        //ourModel.isSelected(true);
        ourModel.model = glm::mat4(1.0f);
        ourModel.model = glm::translate(ourModel.model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        ourModel.model = glm::scale(ourModel.model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        ourModel.Draw(nanoSuitShader, outlineShader);

        //VEGETATION
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < vegetation.size(); i++)
        {
            float distance = glm::length(mainCamera.cameraPos - vegetation[i]);
            sorted[distance] = vegetation[i];
        }

        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            vegetationM.model = glm::mat4(1.0f);
            vegetationM.model = glm::translate(vegetationM.model, it->second);
            vegetationM.Draw(transparentShader, outlineShader);
        }

        // SECOND PASS
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        renderFinalPass.DrawFrom(fboSystem.texColorBuffer);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    ourModel.DeleteGPUInfo();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


