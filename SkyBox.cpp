#include "SkyBox.h"

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox()
{
    Texture textureSkybox(faces, TypeTexture::CUBEMAP, true);
    std::vector<Texture> textures;
    textures.push_back(textureSkybox);
    Shader* sh = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
    Material* skyboxMaterial = new Material(sh, textures);
    matHandle = new MaterialHandle();
    matHandle->AddMaterialToList(skyboxMaterial);
    matHandle->shader = sh;

    SetUp();
}

Skybox::Skybox(bool isSkybox)
{
    Shader* sh = new Shader("shaders/sphericalCubemap.vert", "shaders/sphericalCubemap.frag");
    precookFinal = new Shader("shaders/skyboxHDR.vert", "shaders/skyboxHDR.frag");
    Material* skyboxMaterial = new Material(sh);
    matHandle = new MaterialHandle();
    matHandle->AddMaterialToList(skyboxMaterial);
    matHandle->shader = sh;

    SetUp();
}

void Skybox::PreRender()
{
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);  
    matHandle->shader->use();
    matHandle->shader->setInt("equirectangularMap", 0);
    matHandle->shader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture->ID);
}

void Skybox::Render()
{
    matHandle->shader->use(); 
    
    matHandle->shader->setMat4("view", glm::mat4(glm::mat3(matHandle->shader->mainCamera->view)));
    matHandle->shader->setMat4("projection", matHandle->shader->mainCamera->projection);
    // skybox cube
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, matHandle->listMaterials[0]->textures[0].ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Skybox::RenderHDR()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // skybox cube
    glBindVertexArray(VAO); 
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    //glDepthFunc(GL_LESS);
}

void Skybox::SetUp()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Skybox::AddCamera(Camera* cam)
{
    matHandle->shader->AddCamera(cam);
}
