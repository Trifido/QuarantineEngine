#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{ 
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = mat3(model) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0); //mat4(1.0)
}
