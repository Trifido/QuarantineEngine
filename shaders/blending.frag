#version 430 core
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

uniform Material material;

void main()
{

    vec4 result = texture(material.diffuse[0], TexCoords);
    //if(result.a < 0.1)
    //    discard;
    FragColor = result;
}
