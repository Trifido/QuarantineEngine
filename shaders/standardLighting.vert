#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

#define NR_DIR_LIGHTS 2

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosDirLightSpace[NR_DIR_LIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat4 DirlightSpaceMatrix[NR_DIR_LIGHTS];
uniform vec3 viewPos;

uniform int numDirLights;
void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalMatrix * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 T = normalize(vec3(normalMatrix * aTangents));
    vec3 B = normalize(vec3(normalMatrix * aBitangents));
    vec3 N = normalize(vec3(normalMatrix * aNormal));
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos; 

    for(int i = 0; i < numDirLights; i++)
    { 
        vs_out.FragPosDirLightSpace[i] = DirlightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
