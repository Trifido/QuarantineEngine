#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

#define NR_DIR_LIGHTS 8
#define NR_SPOT_LIGHTS 8

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec2 TexCoordsUV;
    vec4 clipSpace;
    vec3 viewVector;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec4 FragPosDirLightSpace[NR_DIR_LIGHTS];
    vec4 FragPosSpotLightSpace[NR_SPOT_LIGHTS];
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform vec3 viewPos;
uniform float tiling;

uniform mat4 DirlightSpaceMatrix[NR_DIR_LIGHTS];
uniform int numDirLights;
uniform mat4 SpotlightSpaceMatrix[NR_SPOT_LIGHTS];
uniform int numSpotLights;

void main()
{
    vs_out.TexCoordsUV = aTexCoords * tiling;
    vs_out.TexCoords = vec2(aPos.x/2.0 + 0.5, aPos.y/2.0 + 0.5) * tiling;
    vs_out.Normal = mat3(model) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.clipSpace = projection * view * model * vec4(aPos.x, -0.947, aPos.z, 1.0);
    vs_out.viewVector = viewPos - vs_out.FragPos;

    vec3 T = normalize(vec3(model * vec4(aTangents,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangents, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));

    mat3 tTBN = transpose(mat3(T, B, N));
    mat3 TBN = mat3(T, B, N);
    vs_out.TangentViewPos = tTBN * viewPos;
    vs_out.TangentFragPos = tTBN * vs_out.FragPos;

    for(int i = 0; i < numDirLights; i++)
    { 
        vs_out.FragPosDirLightSpace[i] = DirlightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }

    for(int i = 0; i < numSpotLights; i++)
    { 
        vs_out.FragPosSpotLightSpace[i] = SpotlightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }
  
    gl_Position = vs_out.clipSpace;//projection * view * vec4(vs_out.FragPos, 1.0);
}
