#version 430 core
layout (location = 0) out vec4 Ambient;
layout (location = 1) out vec4 DiffSpec;
layout (location = 2) out vec4 occlusionTex;

#define NUM_TEXTURES 1
#define NR_DIR_LIGHTS 8
#define NR_POINT_LIGHTS 8
#define NR_SPOT_LIGHTS 8

const float PI = 3.14159265359;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec4 FragPosDirLightSpace[NR_DIR_LIGHTS];
    vec4 FragPosSpotLightSpace[NR_SPOT_LIGHTS];
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

struct DirLight {
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    sampler2D shadowMap;
    bool isCastShadow;
    int shadowMode;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff; 
    float constant;
    float linear;
    float quadratic; 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    sampler2D shadowMap;
    bool isCastShadow;
    int shadowMode;
};

struct PointLight {    
    vec3 position;
    vec3 diffuse;
    float constant;
    float linear;
    float quadratic;
    float far_plane;
    samplerCube shadowCubeMap;
    bool isCastShadow;
    int shadowMode;
};

//IRRADIANCE MAP
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

//MATERIAL
uniform Material material;

//LIGHTS
uniform int numPointLights, numDirLights, numSpotLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform float generalAmbient;

//CAMERA POSITION
uniform vec3 viewPos;

void main()
{
    float distLight = length(pointLights[0].position - viewPos);

    occlusionTex = vec4(gl_FragCoord.z);

    if(distLight > gl_FragCoord.z / gl_FragCoord.w)
        occlusionTex = vec4(0.0, 0.0, 0.0, 1.0);

    Ambient = vec4(1.0, 0.0, 0.0, 1.0);
    DiffSpec = vec4(0.0, 1.0, 0.0, 1.0);
}
