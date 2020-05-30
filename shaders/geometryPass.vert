#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

#define NR_POINT_LIGHTS 4

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    vec4 FragPosPointLightSpace[NR_POINT_LIGHTS];
    vec3 TangentPointLightPos[NR_POINT_LIGHTS];

    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 PointlightSpaceMatrix[NR_POINT_LIGHTS];
uniform vec3 PointlightPosition[NR_POINT_LIGHTS];
uniform int numPointLights;
uniform vec3 viewPos;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz; 
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(vec3(normalMatrix * aTangents));
    vec3 B = normalize(vec3(normalMatrix * aBitangents));
    vec3 N = normalize(vec3(normalMatrix * aNormal));

    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos; 

    vs_out.Normal = normalMatrix * aNormal;

    for(int i = 0; i < numPointLights; i++)
    {
        vs_out.TangentPointLightPos[i] = TBN * PointlightPosition[i];
        vs_out.FragPosPointLightSpace[i] = PointlightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }

    gl_Position = projection * view * worldPos;
}
