#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangents;
layout (location = 4) in vec3 aBitangents;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec3 tangent;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int num_normal;

void main()
{
    if(num_normal > 0)
    {
        vec3 T = normalize(vec3(model * vec4(aTangents, 0.0)));
        vec3 B = normalize(vec3(model * vec4(aBitangents, 0.0)));
        vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
        TBN = mat3(T, B, N);
    }

    //tangent = (view * model * vec4(aTangents, 0)).xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
