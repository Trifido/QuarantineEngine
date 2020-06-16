#pragma once

#ifndef SSBOSYSTEM_H
#define SSBOSYSTEM_H

#include "ContentSSBO.h"

class SSBOSystem
{
private:
    unsigned int ssboID;

public: 
    ssbo_data_t dataSSBO;
    SSBOSystem() {};
    void SetUpSSBOSystem();
    void ConnectSSBOToShader(unsigned int idProgram);
    void UpdateSSBOSystem();
};

#endif
