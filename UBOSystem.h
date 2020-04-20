#pragma once
#ifndef UBOSYSTEM_H
#define UBOSYSTEM_H

#include "HeadersRequeriments.h"

class UBOSystem
{
private:
    unsigned int uniformBlockIndex;
    unsigned int uboID;
public:
    UBOSystem() {}
    void SetUniformBlockIndex(unsigned int ShaderIndex, const char* name);
    void CreateBuffer(std::size_t bufferSize, unsigned int id);
    void StoreData(glm::mat4 matrix, std::size_t dataSize, std::size_t offset=0);
};

#endif
