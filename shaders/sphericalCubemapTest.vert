#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
    localPos = aPos;  
    gl_Position =  projection * view * model * vec4(aPos, 1.0);
}
