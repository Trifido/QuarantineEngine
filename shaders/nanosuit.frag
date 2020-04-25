#version 330 core
out vec4 FragColor;

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
    float refractiveIndex;
    int isAmbientReflective;
    int isAmbientRefractive;
};
 
in vec2 TexCoords;

//MATERIAL
uniform Material material;

void main()
{
    FragColor = vec4(vec3(texture(material.diffuse[0], TexCoords)), 1.0);
}
