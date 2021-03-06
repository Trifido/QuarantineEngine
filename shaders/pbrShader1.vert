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
uniform mat4 DirlightSpaceMatrix[NR_DIR_LIGHTS];
uniform int numDirLights;
uniform mat4 SpotlightSpaceMatrix[NR_SPOT_LIGHTS];
uniform int numSpotLights;

uniform bool isClipPlane;
uniform vec4 clip_plane;

void main()
{
    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = mat3(model) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

    if(isClipPlane == false)
    {
        gl_ClipDistance[0] = dot(model * vec4(aPos, 1.0), clip_plane);
    }

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

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0); //mat4(1.0)
}
