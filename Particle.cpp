#include "Particle.h"

void Particle::UpdateTexture(int numRows, int numCols)
{
    float lifeFactor = elapsedTime / lifeLength;
    int stageCount = numRows * numCols;
    float atlasProgression = lifeFactor * stageCount;
    int currentIndex = (atlasProgression);//floor
    int nextIndex = currentIndex < stageCount - 1 ? currentIndex + 1 : currentIndex;
    float decimal = (int)atlasProgression;

    blendTextValue = atlasProgression - decimal;
    SetTextureOffset(numRows, numCols, currentOffSet, currentIndex);
    SetTextureOffset(numRows, numCols, nextOffSet, nextIndex);
}

void Particle::SetTextureOffset(int numRows, int numCols, glm::vec2 &offset, int index)
{
    int column = index % numRows;
    int row = index / numCols;
    offset = glm::vec2((float)column / numRows, (float)row / numCols);
}

Particle::Particle()
{
    this->position = glm::vec3(0.0f, 1.0f, 0.0f);
    this->velocity = glm::vec3(0.0, 0.5f, 0.0f);
    this->gravityEffect = 1.f;
    this->lifeLength = 10.f;
    this->rotation = 0.f;
    this->scale = 1.f;
}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, float gravity, float life, float rot, float scal, bool isInfinity)
{
    this->position = pos;
    this->velocity = vel;
    this->gravityEffect = gravity;
    this->lifeLength = life;
    this->rotation = rot;
    this->scale = scal;
    this->isInfinity = isInfinity;
}

bool Particle::Update(float deltaTime)
{
    if (!isRender)
    {
        isRender = true;
        deltaTime = 0.f;
    }
    
    velocity.y += gravityValue * gravityEffect * deltaTime;

    glm::vec3 change(velocity);
    
    change *= deltaTime;
    position += change;   
    elapsedTime += deltaTime;

    if (isInfinity)
    {
        if (elapsedTime > elapsedTime)
            elapsedTime = 0.0f;

        return true;
    }
    else
    {
        return lifeLength > elapsedTime;
    }
}
