#pragma once

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
