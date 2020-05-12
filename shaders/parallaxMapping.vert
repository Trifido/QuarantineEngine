#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

out VS_OUT {
    vec3 FragPos; 
    vec2 TexCoords;
    vec3 TangentLightPos;
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

uniform vec3 positionLight;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); 
    vs_out.TexCoords = aTexCoords;

    if(num_normal > 0)
    {
        vec3 T = normalize(mat3(model) * aTangents);
        vec3 B = normalize(mat3(model) * aBitangents);
        vec3 N = normalize(mat3(model) * aNormal);
        mat3 TBN = transpose(mat3(T, B, N));

        vs_out.TangentLightPos = TBN * positionLight;
        vs_out.TangentViewPos = TBN * viewPos;
        vs_out.TangentFragPos = TBN * vs_out.FragPos;
    }
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
