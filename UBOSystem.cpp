#include "UBOSystem.h"

void UBOSystem::SetUniformBlockIndex(unsigned int ShaderIndex, const char* name)
{
    uniformBlockIndex = glGetUniformBlockIndex(ShaderIndex, name);
    glUniformBlockBinding(ShaderIndex, uniformBlockIndex, 0);
}

void UBOSystem::CreateBuffer(std::size_t bufferSize, unsigned int id)
{
    glGenBuffers(1, &uboID);
    glBindBuffer(GL_UNIFORM_BUFFER, uboID);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    // define the range of the buffer that links to a uniform binding point
    glBindBufferRange(GL_UNIFORM_BUFFER, id, uboID, 0, bufferSize);
}

void UBOSystem::StoreData(glm::mat4 matrix, std::size_t dataSize, std::size_t offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, glm::value_ptr(matrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBOSystem::StoreData(float data, std::size_t dataSize, std::size_t offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, uboID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, &data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
