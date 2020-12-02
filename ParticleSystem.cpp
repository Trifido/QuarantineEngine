#include "ParticleSystem.h"


const float ParticleSystem::particleVertex[]
{
//    -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f
         0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

         0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f, 0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, 0.5f,  0.0f,  1.0f, 1.0f
};

void ParticleSystem::UpdateTransformation(glm::vec3 position, float rotation, float scale)
{
    transform->model = glm::mat4(1.0f);
    
    transform->model = glm::translate(transform->model, position);

    transform->model[0][0] = camera->view[0][0];
    transform->model[0][1] = camera->view[1][0];
    transform->model[0][2] = camera->view[2][0];
    transform->model[1][0] = camera->view[0][1];
    transform->model[1][1] = camera->view[1][1];
    transform->model[1][2] = camera->view[2][1];
    transform->model[2][0] = camera->view[0][2];
    transform->model[2][1] = camera->view[1][2];
    transform->model[2][2] = camera->view[2][2];

    transform->model = glm::rotate(transform->model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    transform->model = glm::scale(transform->model, glm::vec3(scale, scale, scale));

    particleShader->setMat4("model", transform->model);
}

void ParticleSystem::UpdateTextureInfo(Particle& particle)
{
    particle.UpdateTexture(numRowTexture);
    particleShader->setVec2("texCurrentOffset", particle.GetCurrentOffset());
    particleShader->setVec2("texNextOffset", particle.GetNextOffset());
    particleShader->setFloat("blendFactor", particle.GetBlendValueTextures());
    particleShader->setFloat("numRows", numRowTexture);
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

ParticleSystem::ParticleSystem()
{
    particleShader = new Shader("shaders/particleShader.vert", "shaders/particleShader.frag");
    transform = new Transform();
    systemCenter = glm::vec3(0.0, 0.0, 0.0);
    gravityMass = 0.5f;
    speed = 15.0f;
    particlesPerSecond = 5.5f;
    lifeLengthParticles = 3.0f;
    particleTexture = new Texture("./resources/cave/PARTICLE_SYSTEMS/fire.png", TypeTexture::DIFFUSE);
    numRowTexture = 8;

    AddPlane(6);
    setupTexture();
}

void ParticleSystem::SetDeltaTime(float* delta)
{
    deltaTime = delta;
}

void ParticleSystem::Render()
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
    while(it != particles.end())
    {
        bool stillAlive = it->Update(*deltaTime);

        if (stillAlive)
        {
            UpdateTextureInfo(*it);
            UpdateTransformation(it->GetPosition(), it->GetRotation(), it->GetScale());
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            ++it;
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

    setupMesh();
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
    float dirX = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.f - 1.f;
    float dirZ = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))* 2.f - 1.f;
    glm::vec3 velocity(dirX, 1, dirZ);
    velocity = glm::normalize(velocity);
    velocity *= speed;

    particles.push_back(Particle(glm::vec3(center), velocity, gravityMass, lifeLengthParticles, 0.3f, 4.f));
}

void ParticleSystem::setupTexture()
{
    particleShader->use();
    particleShader->setInt("particleTexture", 0);
}
