#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out VS_OUT {
    vec3 color;
    float time;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    float time;
};

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    vs_out.color = aColor;
    vs_out.time = 1;
} 
