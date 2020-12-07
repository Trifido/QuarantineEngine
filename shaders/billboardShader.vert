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
uniform vec4 textureOffset;
uniform vec3 blendFactorRow;

void main()
{
    vs_out.texCurrentCoord.x = aTexCoords.x / blendFactorRow.y +  textureOffset.x;
    vs_out.texCurrentCoord.y = aTexCoords.y / blendFactorRow.z +  textureOffset.y;
    //vs_out.texCurrentCoord = (aTexCoords / blendFactorRow.y) + textureOffset.xy;
    //vs_out.texNextCoord = (aTexCoords / blendFactorRow.y) + textureOffset.zw;
    vs_out.texNextCoord.x = (aTexCoords.x / blendFactorRow.y) + textureOffset.z;
    vs_out.texNextCoord.y = (aTexCoords.y / blendFactorRow.z) + textureOffset.w;
    vs_out.blend = blendFactorRow.x;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
