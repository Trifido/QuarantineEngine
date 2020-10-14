#version 430 core
layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
} fs_in;

layout (std430, binding=2) buffer shader_data
{ 
  vec3 camera_position; 
};

void main()
{
    vec3 albedo = vec3(0.0, 1.0, 0.0);
    FragColor = vec4(albedo, 1.0);
}
