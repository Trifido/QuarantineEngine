#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define NUM_TEXTURES 2

const float PI = 3.14159265359;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

layout (std430, binding=2) buffer shader_data
{ 
  vec3 camera_position; 
};

struct Material {
    int num_diffuse;
    int num_specular;
    int num_emissive;
    int num_normal;
    int num_height;
    int num_ao;
    int num_metallic;
    int num_roughness;
    int num_bump;
    int blinn;

    sampler2D diffuse[NUM_TEXTURES];
    sampler2D specular[NUM_TEXTURES];
    sampler2D emissive[NUM_TEXTURES];
    sampler2D height[NUM_TEXTURES];
    sampler2D normal[NUM_TEXTURES];
    sampler2D metallic[NUM_TEXTURES];
    sampler2D ao[NUM_TEXTURES];
    sampler2D roughness[NUM_TEXTURES];
    sampler2D bump[NUM_TEXTURES];

    float heightScale;
    float min_uv;
    float max_uv;
    float shininess;
    float bloomBrightness;

};

//MATERIAL
uniform Material material;

//CAMERA POSITION
uniform vec3 viewPos;

void main()
{
    //GET TEXTURE COORDS 
    vec2 texCoords = fs_in.TexCoords;

    vec3 albedo = pow(texture(material.diffuse[0], texCoords).rgb, vec3(2.2));

    FragColor = vec4(albedo, 1.0);
    BrightColor = vec4(albedo, 1.0);
}
