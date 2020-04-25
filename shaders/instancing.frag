#version 330 core
out vec4 FragColor;

in vec2 TexCoords;


#define NUM_TEXTURES 4 

struct Material {
    int num_diffuse;
    int num_specular;
    int num_emissive;
    int num_normal;
    int num_height;

    sampler2D diffuse[NUM_TEXTURES];
    sampler2D specular[NUM_TEXTURES];
    sampler2D emissive[NUM_TEXTURES];
    sampler2D height[NUM_TEXTURES];
    sampler2D normal[NUM_TEXTURES];
    
    float shininess;
};
//MATERIAL
uniform Material material;

void main()
{
    FragColor = texture(material.diffuse[0], TexCoords);
}
