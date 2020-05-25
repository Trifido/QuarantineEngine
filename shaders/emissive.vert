#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

#define NR_POINT_LIGHTS 1

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    vec4 FragPosPointLightSpace[NR_POINT_LIGHTS];
    vec3 TangentPointLightPos[NR_POINT_LIGHTS];

    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform int num_normal; 
uniform mat4 PointlightSpaceMatrix[NR_POINT_LIGHTS];
uniform vec3 PointlightPosition[NR_POINT_LIGHTS];

uniform vec3 viewPos;

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

    vs_out.TangentPointLightPos[0] = TBN * PointlightPosition[0];
    vs_out.FragPosPointLightSpace[0] = PointlightSpaceMatrix[0] * vec4(vs_out.FragPos, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
