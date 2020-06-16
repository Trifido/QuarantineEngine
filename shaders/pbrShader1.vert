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
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform vec3 viewPos;

void main()
{ 
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = mat3(model) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 T = normalize(vec3(model * vec4(aTangents,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangents, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));

    mat3 tTBN = transpose(mat3(T, B, N));
    mat3 TBN = mat3(T, B, N);
    vs_out.TangentViewPos = tTBN * viewPos;
    vs_out.TangentFragPos = tTBN * vs_out.FragPos;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}
