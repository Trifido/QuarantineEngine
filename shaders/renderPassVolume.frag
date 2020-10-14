#version 430 core

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D DiffSpecTex;
  
in vec2 TexCoords;
uniform float gamma; 
uniform float exposure;

void main() {
    vec4 diffSpec = texelFetch(DiffSpecTex, ivec2(gl_FragCoord), 0);
    vec3 mapped = vec3(1.0) - exp(-diffSpec.rgb * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}
