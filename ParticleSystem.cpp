﻿#include "ParticleSystem.h"


const float ParticleSystem::particleVertex[]
{
         0.5f, -0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,

         0.5f, -0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f, 0.5f,  0.0f,  0.0f, 0.0f,
         0.5f, 0.5f,  0.0f,  1.0f, 0.0f
};

ParticleSystem::ParticleSystem()
{
    type = ParticleSystemType::COMMON_PS;
    particleShader = new Shader("shaders/particleShader.vert", "shaders/particleShader.frag");
    transform = new CustomTransform();
    systemCenter = glm::vec3(0.0, 0.0, 0.0);
    gravityMass = 0.5f;
    speed = 15.0f;
    particlesPerSecond = 5.5f;
    lifeLengthParticles = 3.0f;
    particleScale = 4.0f;
    particleRotation = 0.0f;
    particleTexture = new Texture("./resources/cave/PARTICLE_SYSTEMS/fire.png", TypeTexture::DIFFUSE);
    numRowTexture = numColTexture = 8;
    emitRadius = 0.0f;
    emitAngle = 0.0f;
    isColorLife = false;

    instancedAttributes = new InstanceParticleAttr[amount];

    AddPlane(6);
    setupMesh();
    setupInstanceMesh();
    setupTexture();
}

ParticleSystem::ParticleSystem(ParticleSystemType type, std::string nameTexture, int numRows, int numCols)
{
    this->type = type;
    particleTexture = new Texture(nameTexture, TypeTexture::DIFFUSE);
    transform = new CustomTransform();
    systemCenter = glm::vec3(0.0, 0.0, 0.0);
    gravityMass = 0.5f;
    speed = 15.0f;
    particleScale = 4.0f;
    particlesPerSecond = 5.5f;
    lifeLengthParticles = 3.0f;
    particleRotation = 0.0f;
    numRowTexture = numRows;
    numColTexture = numCols;
    emitRadius = 0.0f;
    emitAngle = 0.0f;
    isColorLife = false;

    AddPlane(6);
    setupMesh();

    switch (this->type)
    {
        case ParticleSystemType::BILLBOARD:
            isInfinity = true;
            particles.push_back(Particle(systemCenter, glm::vec3(0.0), 0.0, 10, particleRotation, particleScale, isInfinity));
            instancedAttributes = new InstanceParticleAttr[1];
            particleShader = new Shader("shaders/billboardShader.vert", "shaders/billboardShader.frag");
            break;
        case ParticleSystemType::BILLBOARD_ANIMATED:
            isInfinity = false;
            particles.push_back(Particle(systemCenter, glm::vec3(0.0), 0.0, 1, particleRotation, particleScale, isInfinity));
            instancedAttributes = new InstanceParticleAttr[1];
            particleShader = new Shader("shaders/billboardShader.vert", "shaders/billboardShaderAnim.frag");
            break;
        case ParticleSystemType::VOLUMEN_PS:
            instancedAttributes = new InstanceParticleAttr[amount];
            particleShader = new Shader("shaders/particleShader.vert", "shaders/particleShader.frag");
            setupInstanceMesh();
            break;
        case ParticleSystemType::COMMON_PS:
        default:
            instancedAttributes = new InstanceParticleAttr[amount];
            particleShader = new Shader("shaders/particleShader.vert", "shaders/particleShader.frag");
            setupInstanceMesh();
            break;
    }

    setupTexture();
}

void ParticleSystem::UpdateParticleAttributes(int id, Particle& particleInstanced)
{
    if (id > -1 && id < amount)
    {
        transform->model = glm::mat4(1.0f);

        transform->model = glm::translate(transform->model, particleInstanced.GetPosition());

        transform->model[0][0] = camera->view[0][0];
        transform->model[0][1] = camera->view[1][0];
        transform->model[0][2] = camera->view[2][0];
        transform->model[1][0] = camera->view[0][1];
        transform->model[1][1] = camera->view[1][1];
        transform->model[1][2] = camera->view[2][1];
        transform->model[2][0] = camera->view[0][2];
        transform->model[2][1] = camera->view[1][2];
        transform->model[2][2] = camera->view[2][2];

        transform->model = glm::rotate(transform->model, particleInstanced.GetRotation(), glm::vec3(0.0f, 0.0f, 1.0f));
        transform->model = glm::scale(transform->model, glm::vec3(particleInstanced.GetScale()));

        particleInstanced.UpdateTexture(numRowTexture, numColTexture);

        instancedAttributes[id].modelInstance = transform->model;
        instancedAttributes[id].textureOffset = glm::vec4(particleInstanced.GetCurrentOffset(), particleInstanced.GetNextOffset());
        instancedAttributes[id].blendFactorRow = glm::vec4(particleInstanced.GetBlendValueTextures(), numRowTexture, numColTexture, isColorLife);
        instancedAttributes[id].lifeColor = *particleInstanced.GetColor();

        if (type != ParticleSystemType::BILLBOARD && type != ParticleSystemType::BILLBOARD_ANIMATED)
        {
            glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, (id * sizeof(InstanceParticleAttr)), sizeof(glm::vec4), &instancedAttributes[id].textureOffset);
            glBufferSubData(GL_ARRAY_BUFFER, offsetof(InstanceParticleAttr, blendFactorRow) + (id * sizeof(InstanceParticleAttr)), sizeof(glm::vec4), &instancedAttributes[id].blendFactorRow);
            glBufferSubData(GL_ARRAY_BUFFER, offsetof(InstanceParticleAttr, modelInstance) + (id * sizeof(InstanceParticleAttr)), sizeof(glm::mat4), &instancedAttributes[id].modelInstance);
            glBufferSubData(GL_ARRAY_BUFFER, offsetof(InstanceParticleAttr, lifeColor) + (id * sizeof(InstanceParticleAttr)), sizeof(glm::vec4), &instancedAttributes[id].lifeColor);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else
        {
            particleShader->use();
            particleShader->setMat4("model", instancedAttributes[id].modelInstance);
            particleShader->setVec4("textureOffset", instancedAttributes[id].textureOffset);
            particleShader->setVec4("blendFactorRow", instancedAttributes[id].blendFactorRow);
            //particleShader->setVec4("lifeColor", instancedAttributes[id].lifeColor);
        }
    }
}

std::list< Particle> ParticleSystem::MergeSort(std::list<Particle> mergeList)
{
    if (mergeList.size() <= 1)
        return mergeList;

    std::list<Particle> left;
    std::list<Particle> right;

    std::list<Particle>::iterator it;
    unsigned int id = 0;
    for (it = mergeList.begin(); it != mergeList.end(); it++, id++)
    {
        if (id < mergeList.size() / 2)
        {
            left.push_back(*it);
        }
        else
        {
            right.push_back(*it);
        }
    }

    left = MergeSort(left);
    right = MergeSort(right);

    return Merge(left, right);
}

std::list<Particle> ParticleSystem::Merge(std::list<Particle> &left, std::list<Particle> &right)
{
    std::list<Particle> result;

    std::list<Particle>::iterator itLeft = left.begin();
    std::list<Particle>::iterator itRight = right.begin();

    while (!left.empty() && !right.empty())
    {
        float distanceLeft = glm::distance(camera->cameraPos, itLeft->GetPosition());
        float distanceRight = glm::distance(camera->cameraPos, itRight->GetPosition());

        if (distanceLeft < distanceRight)
        {
            result.push_back(*itRight);
            itRight = right.erase(itRight);
        }
        else
        {
            result.push_back(*itLeft);
            itLeft = left.erase(itLeft);
        }
    }

    while (!right.empty())
    {
        result.push_back(*itRight);
        itRight = right.erase(itRight);
    }

    while (!left.empty())
    {
        result.push_back(*itLeft);
        itLeft = left.erase(itLeft);
    }

    return result;
}

void ParticleSystem::SetDeltaTime(float* delta)
{
    deltaTime = delta;
}

void ParticleSystem::Render()
{
    if (type != ParticleSystemType::BILLBOARD && type != ParticleSystemType::BILLBOARD_ANIMATED)
    {
        GenerateParticles();

        particleShader->use();

        glBindVertexArray(VAO);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particleTexture->ID);

        //Para cada particula itero y ordeno la lista
        particles = MergeSort(particles);

        std::list<Particle>::iterator it = particles.begin();
        int id = 0;
        while (it != particles.end())
        {
            bool stillAlive = it->Update(*deltaTime);

            if (stillAlive)
            {
                UpdateColorLife(*it);
                UpdateParticleAttributes(id, *it);
                ++it;
                ++id;
            }
            else
            {
                it = particles.erase(it);
            }
        }

        if (!particles.empty())
            glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, particles.size());

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        particleShader->unuse();
    }
    else
    {
        particleShader->use();

        glBindVertexArray(VAO);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particleTexture->ID);

        std::list<Particle>::iterator it = particles.begin();

        if (!particles.empty())
        {
            bool stillAlive = it->Update(*deltaTime);

            if (stillAlive)
            {
                UpdateParticleAttributes(0, *it);
                glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            }
            else
            {
                it = particles.erase(it);
            }
        }

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        particleShader->unuse();
    }
}

void ParticleSystem::AddCamera(Camera* mainCamera)
{
    camera = mainCamera;
    particleShader->AddCamera(camera);
}

void ParticleSystem::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexParticle), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), (void*)0);
    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), (void*)offsetof(VertexParticle, TexCoords));

    glBindVertexArray(0);
}

void ParticleSystem::setupInstanceMesh()
{
    glGenBuffers(1, &instanceBuffer);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(InstanceParticleAttr), &instancedAttributes[0], GL_STREAM_DRAW);

    //vec4 for current texture offset and next texture offset
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceParticleAttr), (void*)0);
    //vec2 for blending and row factor
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceParticleAttr), (void*)offsetof(InstanceParticleAttr, blendFactorRow));
    // set attribute pointers for model matrix (4 times vec4)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceParticleAttr), (void*)offsetof(InstanceParticleAttr, modelInstance));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceParticleAttr), (void*)(offsetof(InstanceParticleAttr, modelInstance) + sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceParticleAttr), (void*)(offsetof(InstanceParticleAttr, modelInstance) + 2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceParticleAttr), (void*)(offsetof(InstanceParticleAttr, modelInstance) + 3 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceParticleAttr), (void*)(offsetof(InstanceParticleAttr, modelInstance) + 4 * sizeof(glm::vec4)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
    glVertexAttribDivisor(8, 1);

    glBindVertexArray(0);
}

void ParticleSystem::DeleteGPUInfo()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void ParticleSystem::AddPlane(unsigned int numVertex)
{
    const int NUMCOMP = 5;
    for (int i = 0; i < numVertex; i++)
    {
        unsigned int index = i * NUMCOMP;

        VertexParticle vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = particleVertex[index];
        vector.y = particleVertex[index + 1];
        vector.z = particleVertex[index + 2];
        vertex.Position = vector;

        glm::vec2 vec;
        vec.x = particleVertex[index + 3];
        vec.y = particleVertex[index + 4];
        vertex.TexCoords = vec;

        vertices.push_back(vertex);
        indices.push_back(i);
    }
}

void ParticleSystem::GenerateParticles()
{
    float particlesToCreate = particlesPerSecond * *deltaTime;
    int count = (int)floor(particlesToCreate);
    int entero = (int)particlesToCreate;
    float partialParticle = particlesToCreate - entero;

    for (int i = 0; i < count; i++) {
        EmitParticle(systemCenter);
    }
    if ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) < partialParticle) {
        EmitParticle(systemCenter);
    }
}

void ParticleSystem::EmitParticle(glm::vec3 center) {
    glm::vec3 emitCone = glm::vec3(0.0, 1.0, 0.0);
    if (emitAngle > 0)
    {
        float dirX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * emitAngle - (emitAngle/2.0f);
        float dirZ = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * emitAngle - (emitAngle/2.0f);
        emitCone = glm::vec3(dirX, 1.0, dirZ);
    }

    glm::vec3 velocity = glm::normalize(emitCone);
    velocity *= speed;

    glm::vec3 emitCenter = center;

    if (emitRadius > 0)
    {
        float xPos = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * emitRadius - (emitRadius/2.0f);
        float zPos = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * emitRadius - (emitRadius/2.0f);
        emitCenter += glm::vec3(xPos, 0.0, zPos);
    }

    float rotation = particleRotation;
    if (isRandomRotation)
    {
        rotation = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * randomRotationParticles;// -(emitRadius / 2.0f);
    }

    particles.push_back(Particle(emitCenter, velocity, gravityMass, lifeLengthParticles, rotation, particleScale, isInfinity));
}

void ParticleSystem::setupTexture()
{
    particleShader->use();
    particleShader->setInt("particleTexture", 0);
}

void ParticleSystem::UpdatePosition()
{
    if (type == ParticleSystemType::BILLBOARD || type == ParticleSystemType::BILLBOARD_ANIMATED)
    {
        std::list<Particle>::iterator it = particles.begin();
        it->SetPostion(systemCenter);
    }
}

void ParticleSystem::UpdateRotation()
{
    if (type == ParticleSystemType::BILLBOARD || type == ParticleSystemType::BILLBOARD_ANIMATED)
    {
        std::list<Particle>::iterator it = particles.begin();
        it->SetRotation(particleRotation);
    }
}

void ParticleSystem::UpdateScale()
{
    if (type == ParticleSystemType::BILLBOARD || type == ParticleSystemType::BILLBOARD_ANIMATED)
    {
        std::list<Particle>::iterator it = particles.begin();
        it->SetScale(particleScale);
    }
}

void ParticleSystem::SetPosition(glm::vec3 pos)
{
    systemCenter = pos;
    UpdatePosition();
}

void ParticleSystem::SetScale(float scal)
{
    particleScale = scal;
    UpdateScale();
}

void ParticleSystem::SetRotation(float rot)
{
    particleRotation = rot;
    UpdateRotation();
}

void ParticleSystem::SetProperties(float pps, float lifep, float speedp, float gravityp, float radius, float angle)
{
    this->particlesPerSecond = pps;
    this->lifeLengthParticles = lifep;
    this->speed = speedp;
    this->gravityMass = gravityp;
    this->emitRadius = radius;
    this->emitAngle = angle;
}

void ParticleSystem::AddColorLife(ParticleLifeColor pColor)
{
    isColorLife = true;
    if(pColor.isColor)
        colorLife.insert(std::pair<float, ParticleLifeColor>(pColor.percentageTime, pColor));
    if(pColor.isAlpha)
        alphaLife.insert(std::pair<float, float>(pColor.percentageTime, pColor.color.a));
}

void ParticleSystem::UpdateColorLife(Particle &particle)
{
    float percentageLife = (particle.GetElapsedTime() / lifeLengthParticles) * 100.0f;
    if (isColorLife && !colorLife.empty())
    {
        if (colorLife.size() == 1)
        {
            particle.SetColor(colorLife.begin()->second.color);
        }
        else
        {
            glm::vec3 firstColor, secondColor;
            float firstAlpha, secondAlpha;
            float timeFirstColor, timeSecondColor, timeFirstAlpha, timeSecondAlpha;

            std::map<float, ParticleLifeColor>::iterator itlowColor, itupColor;
            std::map<float, float>::iterator itlowAlpha, itupAlpha;

            itlowColor = (percentageLife > 0.0f) ? --colorLife.lower_bound(percentageLife) : colorLife.lower_bound(percentageLife);
            itupColor = (percentageLife == 0.0f) ? ++colorLife.lower_bound(percentageLife) : colorLife.lower_bound(percentageLife);
            itlowAlpha = (percentageLife > 0.0f) ? --alphaLife.lower_bound(percentageLife) : alphaLife.lower_bound(percentageLife);
            itupAlpha = (percentageLife == 0.0f) ? ++alphaLife.lower_bound(percentageLife) : alphaLife.lower_bound(percentageLife);

            if (itupColor == colorLife.end()) --itupColor;
            if (itupAlpha == alphaLife.end()) --itupAlpha;

            firstColor = itlowColor->second.color;
            secondColor = itupColor->second.color;
            firstAlpha = itlowAlpha->second;
            secondAlpha = itupAlpha->second;

            timeFirstColor = itlowColor->first;
            timeSecondColor = itupColor->first;
            timeFirstAlpha = itlowAlpha->first;
            timeSecondAlpha = itupAlpha->first;

            glm::vec3 resultColor = firstColor + (((secondColor - firstColor) / (timeSecondColor - timeFirstColor)) * (percentageLife - timeFirstColor));
            float resultAlpha = firstAlpha + (((secondAlpha - firstAlpha) / (timeSecondAlpha - timeFirstAlpha)) * (percentageLife - timeFirstAlpha));
            particle.SetColor(glm::vec4(resultColor, resultAlpha));
        }
    }
}

void ParticleSystem::SetRandomRotation(float randomRot)
{
    isRandomRotation = (randomRot > 0.0);

    if (isRandomRotation)
    {
        randomRotationParticles = randomRot * glm::pi<float>() / 180.0f;
    }
}
