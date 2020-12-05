#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 textureOffset;
layout (location = 3) in vec2 blendFactorRow;
layout (location = 4) in mat4 modelInstance;

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

//uniform mat4 model;
//uniform vec2 texCurrentOffset;
//uniform vec2 texNextOffset;
//uniform float blendFactor;
//uniform float numRows;

void main()
{
    vs_out.texCurrentCoord = (aTexCoords / blendFactorRow.y) + textureOffset.xy;
    vs_out.texNextCoord = (aTexCoords / blendFactorRow.y) + textureOffset.zw;
    vs_out.blend = blendFactorRow.x;
    gl_Position = projection * view * modelInstance * vec4(aPos, 1.0);
}
