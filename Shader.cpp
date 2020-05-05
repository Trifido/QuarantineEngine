#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertexShader, fragmentShader;

    //COMPILAMOS EL SHADER DE VERTICES 
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    //Comprobamos que se ha compilado el shader
    CheckVertexCompilation(vertexShader);

    //COMPILAMOS EL SHADER DE FRAGMENTOS 
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    //Comprobamos que se ha compilado el shader
    CheckFragmentCompilation(fragmentShader);

    //COMPILAMOS EL SHADER PROGRAM
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    //Linkamos el shader program
    glLinkProgram(ID);
    //Comprobamos que se ha linkado el programa
    CheckProgramLink(ID);

    //Borramos los shaders object, ya estan linkados al programa
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string geometryCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream gShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        gShaderFile.open(geometryPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, gShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        gShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        geometryCode = gShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertexShader, geometryShader, fragmentShader;

    //COMPILAMOS EL SHADER DE VERTICES 
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    //Comprobamos que se ha compilado el shader
    CheckVertexCompilation(vertexShader);

    //COMPILAMOS EL SHADER DE GEOMETRÏA 
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &gShaderCode, NULL);
    glCompileShader(geometryShader);
    //Comprobamos que se ha compilado el shader
    CheckGeometryCompilation(geometryShader);

    //COMPILAMOS EL SHADER DE FRAGMENTOS 
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    //Comprobamos que se ha compilado el shader
    CheckFragmentCompilation(fragmentShader);

    //COMPILAMOS EL SHADER PROGRAM
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, geometryShader);
    glAttachShader(ID, fragmentShader);
    //Linkamos el shader program
    glLinkProgram(ID);
    //Comprobamos que se ha linkado el programa
    CheckProgramLink(ID);

    //Borramos los shaders object, ya estan linkados al programa
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::AddCamera(Camera* cam)
{
    mainCamera = cam;
}

void Shader::setBool(const std::string& name, bool value) const 
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, glm::vec3& pos) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &pos[0]);
}

void Shader::setMat3(const std::string& name, glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::AddTexture(Texture* tex, const std::string& name)
{
    numTextures++;

    if (name.c_str() != NULL)
        nameTexture.push_back(name);
    else
        nameTexture.push_back("texture" + std::to_string(numTextures));

    textures.push_back(tex);
}

void Shader::ActivateTextures()
{
    glUseProgram(ID);
    std::string nameTex;

    for (unsigned int i = 0; i < numTextures; i++)
    {
        nameTex = nameTexture.at(i);
        this->setInt(nameTex, i);
    }
}

void Shader::AddLight(Light* light)
{
    lights.push_back(light);
}

void Shader::AddLight(std::vector<Light*> lightVec)
{
    lights = lightVec;
}

void Shader::ActivateLights()
{
    int numPointLights = 0;
    int numSpotLights = 0;
    int numDirLights = 0;
    int numFPSLights = 0;
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights[i]->GetType() == TypeLight::DIRL)
        {
            this->setVec3("dirLight.ambient", lights[i]->GetAmbient());
            this->setVec3("dirLight.diffuse", lights[i]->GetDiffuse());
            this->setVec3("dirLight.specular", lights[i]->GetSpecular());
            this->setVec3("dirLight.direction", lights[i]->GetDirection());
            numDirLights++;
        }
        else if (lights[i]->GetType() == TypeLight::POINTLIGHT)
        {
            std::string name = "pointLights[" + std::to_string(numPointLights) + "].";
            this->setVec3(name + "position", lights[i]->GetPosition());
            this->setVec3(name + "ambient", lights[i]->GetAmbient());
            this->setVec3(name + "diffuse", lights[i]->GetDiffuse());
            this->setVec3(name + "specular", lights[i]->GetSpecular());
            this->setFloat(name + "constant", lights[i]->GetConstant());
            this->setFloat(name + "linear", lights[i]->GetLinear());
            this->setFloat(name + "quadratic", lights[i]->GetQuadratic());

            numPointLights++;
        }
        else if (lights[i]->GetType() == TypeLight::SPOTL)
        {
            std::string name = "spotLights[" + std::to_string(numSpotLights) + "].";
            this->setVec3(name + "position", lights[i]->GetPosition());
            this->setVec3(name + "direction", lights[i]->GetDirection());
            this->setVec3(name + "ambient", lights[i]->GetAmbient());
            this->setVec3(name + "diffuse", lights[i]->GetDiffuse());
            this->setVec3(name + "specular", lights[i]->GetSpecular());
            this->setFloat(name + "constant", lights[i]->GetConstant());
            this->setFloat(name + "linear", lights[i]->GetLinear());
            this->setFloat(name + "quadratic", lights[i]->GetQuadratic());
            this->setFloat(name + "cutOff", lights[i]->GetCutOff());
            this->setFloat(name + "outerCutOff", lights[i]->GetOuterCutOff());

            numSpotLights++;
        }
        else
        {
            std::string name = "fpsSpotLight.";
            this->setVec3(name + "position", mainCamera->cameraPos);
            this->setVec3(name + "direction", mainCamera->cameraFront);
            this->setVec3(name + "ambient", lights[i]->GetAmbient());
            this->setVec3(name + "diffuse", lights[i]->GetDiffuse());
            this->setVec3(name + "specular", lights[i]->GetSpecular());
            this->setFloat(name + "constant", lights[i]->GetConstant());
            this->setFloat(name + "linear", lights[i]->GetLinear());
            this->setFloat(name + "quadratic", lights[i]->GetQuadratic());
            this->setFloat(name + "cutOff", lights[i]->GetCutOff());
            this->setFloat(name + "outerCutOff", lights[i]->GetOuterCutOff());
            numFPSLights++;
        }
    }
    //Send total number of spot and point lights
    this->setInt("numSpotLights", numSpotLights);
    this->setInt("numPointLights", numPointLights);
    this->setInt("numDirLights", numDirLights);
    this->setInt("numFPSLights", numFPSLights);
}

void Shader::ActivateCamera()
{
    this->setVec3("viewPos", mainCamera->cameraPos);
}

void CheckVertexCompilation(unsigned int vertexShader)
{
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
        fprintf(stderr, infoLog);
    }
}

void CheckGeometryCompilation(unsigned int geoShader)
{
    int  success;
    char infoLog[512];
    glGetShaderiv(geoShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(geoShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n");
        fprintf(stderr, infoLog);
    }
}


void CheckFragmentCompilation(unsigned int fragShader)
{
    int  success;
    char infoLog[512];
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
        fprintf(stderr, infoLog);
    }
}

void CheckProgramLink(unsigned int shaderProgram)
{
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::LINK_SHADER_PROGRAM\n");
        fprintf(stderr, infoLog);
    }
}
