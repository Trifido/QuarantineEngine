#include "Model.h"

Model::Model()
{
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    //float vertex[] = {
    //    // positions          // colors           // texture coords
    //     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
    //     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    //    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    //    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    //};

    //unsigned int index[] = {  // note that we start from 0!
    //    0, 1, 3,   // first triangle
    //    1, 2, 3    // second triangle
    //};

    //float uvs[] = {
    //    1.0f, 1.0f,   // top-right corner
    //    1.0f, 0.0f,  // lower-right corner
    //    0.0f, 0.0f,  // lower-left corner  
    //    0.0f, 1.0f   // top-left corner
    //};

    //vertices = vertex;

    //indices = index;

    //texCoords = uvs;

}

void Model::Rotation(float radians, glm::vec3 axis)
{
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(radians), axis);
}

void Model::TranslationTo(glm::vec3 position)
{
    this->model = glm::translate(this->model, position);
}

void Model::ScaleTo(glm::vec3 scale)
{
    this->model = glm::scale(this->model, scale);
}

void Model::ResetModel()
{
    this->model = glm::mat4(1.0f);
}
