#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 textureOffset;
layout (location = 3) in vec4 blendFactorRow;
layout (location = 4) in mat4 modelInstance;
layout (location = 8) in vec4 lifeColor;

out VS_OUT {
    vec2 texCurrentCoord;
    vec2 texNextCoord;
    float blend;
    float isLife;
    vec4 color;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

//uniform mat4 model;
//uniform vec2 texCurrentOffset;
//uniform vec2 texNextOffset;
//uniform float blendFactor;
//uniform float numRows;

void main()
{
    vs_out.texCurrentCoord.x = aTexCoords.x / blendFactorRow.y +  textureOffset.x;
    vs_out.texCurrentCoord.y = aTexCoords.y / blendFactorRow.z +  textureOffset.y;
    vs_out.texNextCoord.x = (aTexCoords.x / blendFactorRow.y) + textureOffset.z;
    vs_out.texNextCoord.y = (aTexCoords.y / blendFactorRow.z) + textureOffset.w;
    vs_out.blend = blendFactorRow.x;
    vs_out.isLife = blendFactorRow.w;
    vs_out.color = lifeColor;
    gl_Position = projection * view * modelInstance * vec4(aPos, 1.0);
}
