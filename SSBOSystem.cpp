#include "SSBOSystem.h"

#include "HeadersRequeriments.h"

void SSBOSystem::SetUpSSBOSystem()
{
    glGenBuffers(1, &ssboID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(dataSSBO), &dataSSBO, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBOSystem::ConnectSSBOToShader(unsigned int idProgram)
{
    unsigned int block_index = 0;
    block_index = glGetProgramResourceIndex(idProgram, GL_SHADER_STORAGE_BLOCK, "shader_data");

    GLuint ssbo_binding_point_index = 2;
    glShaderStorageBlockBinding(idProgram, block_index, ssbo_binding_point_index);

    //GLuint binding_point_index = 80;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, ssboID);
}

void SSBOSystem::UpdateSSBOSystem()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p, &dataSSBO, sizeof(dataSSBO));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
