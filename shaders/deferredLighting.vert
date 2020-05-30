#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

#define NR_POINT_LIGHTS 4

out VS_OUT {
    vec2 TexCoords;
    //vec3 TangentPointLightPos[NR_POINT_LIGHTS];
} vs_out;

uniform mat4 PointlightSpaceMatrix[NR_POINT_LIGHTS];
uniform vec3 PointlightPosition[NR_POINT_LIGHTS];
uniform int numPointLights;

void main()
{
    vs_out.TexCoords = aTexCoords;

    //for(int i = 0; i < numPointLights; i++)
    //{
    //    vs_out.TangentPointLightPos[i] = TBN * PointlightPosition[i]; 
    //}

    gl_Position = vec4(aPos, 1.0);
}
