#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

#define NR_LIGHTS 5

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosDirLightSpace[NR_LIGHTS];
    vec4 FragPosPointLightSpace[NR_LIGHTS];
    mat3 TBN;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform int num_normal; 
uniform mat4 DirlightSpaceMatrix[NR_LIGHTS];
uniform mat4 PointlightSpaceMatrix[NR_LIGHTS];

uniform int numSpotLights;
uniform int numPointLights;
uniform int numDirLights;
uniform int numFPSLights;

void main()
{
    if(num_normal > 0)
    {
        vec3 T = normalize(vec3(model * vec4(aTangents, 0.0)));
        vec3 B = normalize(vec3(model * vec4(aBitangents, 0.0)));
        vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
        vs_out.TBN = mat3(T, B, N);
    }

    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    for(int i = 0; i < numDirLights; i++)
        vs_out.FragPosDirLightSpace[i] = DirlightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    for(int i = 0; i < numPointLights; i++)
        vs_out.FragPosPointLightSpace[i] = PointlightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
