#version 330 core
layout (location = 0) out vec3 gVolumeLightValue;

in VS_OUT {
    vec3 FragPos;
} fs_in;

void main()
{
    gVolumeLightValue = fs_in.FragPos;
}
