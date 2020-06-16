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
    mat3 TBN;
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
    vec4 worldPos2 = view * model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos2.xyz; 
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    //vec3 T = normalize(vec3(normalMatrix * aTangents));
    //vec3 B = normalize(vec3(normalMatrix * aBitangents));
    //vec3 N = normalize(vec3(normalMatrix * aNormal));
    vec3 T = normalize(vec3(model * vec4(aTangents,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangents, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));

    mat3 tTBN = transpose(mat3(T, B, N));
    mat3 TBN = mat3(T, B, N);
    vs_out.TangentViewPos = tTBN * viewPos;
    vs_out.TangentFragPos = tTBN * vs_out.FragPos;
    vs_out.TBN = TBN;

    mat3 normalMatrix2 = transpose(inverse(mat3(view * model)));
    vs_out.Normal = aNormal;
    vs_out.Normal = normalMatrix2 * aNormal;

    for(int i = 0; i < numPointLights; i++)
    {
        vs_out.TangentPointLightPos[i] = tTBN * PointlightPosition[i];
        vs_out.FragPosPointLightSpace[i] = PointlightSpaceMatrix[i] * vec4(vs_out.FragPos, 1.0);
    }

    gl_Position = projection * view * worldPos;
}
