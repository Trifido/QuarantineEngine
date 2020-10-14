#version 430 core
layout (location = 0) in vec3 aPos;

out VS_OUT {
    vec3 FragPos;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{ 
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}
