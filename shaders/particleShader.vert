#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    vec2 texCurrentCoord;
    vec2 texNextCoord;
    float blend;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform vec2 texCurrentOffset;
uniform vec2 texNextOffset;
uniform float blendFactor;
uniform float numRows;

void main()
{
    vs_out.texCurrentCoord = aTexCoords / numRows + texCurrentOffset;
    vs_out.texNextCoord = aTexCoords / numRows + texNextOffset;
    vs_out.blend = blendFactor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
