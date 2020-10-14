#version 430 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out VS_OUT {
    vec3 VPosition;
} vs_out;

uniform mat4 model;

void main()
{ 
    vs_out.VPosition = vec3(view * model * vec4(aPos, 1.0));
    //mat4 mvp = projection * view * model;
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
}
