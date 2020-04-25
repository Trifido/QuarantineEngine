#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 TexCoords;
    float time;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
    float time;
};

uniform mat4 model; 

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.time = time;
    vec3 FragPos1 = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos1, 1.0);
}
