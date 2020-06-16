#version 330 core
layout (location = 0) in vec3 aPos;

out VS_OUT {
    vec3 FragPos;
} vs_out;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz; 

    gl_Position = projection * view * worldPos;
}
