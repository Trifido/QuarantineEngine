#version 430 core
layout (location = 0) out vec4 occlusionTex;

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
    int num_normal;
    int num_height;
    int num_bump;
    int num_diffuse;

    sampler2D height[NUM_TEXTURES];
    sampler2D diffuse[NUM_TEXTURES];
    sampler2D normal[NUM_TEXTURES];
    sampler2D bump[NUM_TEXTURES];

    float heightScale;
    float min_uv;
    float max_uv;
};

struct DirLight {
    vec3 position;
    vec3 direction;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
};

struct PointLight {    
    vec3 position;
};

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

//-- PARALLAX FUNCTION --
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

vec3 getNormalFromMap(vec2 TexCoords)
{
    vec3 tangentNormal = texture(material.normal[0], TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    //GET TEXTURE COORDS 
    vec2 texCoords;

    if(material.num_height > 0)
        texCoords = ParallaxMapping(fs_in.TexCoords,  normalize(fs_in.TangentViewPos - fs_in.TangentFragPos));
    else
        texCoords = fs_in.TexCoords;

    if(texture(material.diffuse[0], texCoords).a < 0.1)
        discard; 

    vec3 N;
    if(material.num_normal > 0)
        N = getNormalFromMap(texCoords);
    else if(material.num_bump > 0)
        N = normalize(fs_in.Normal * texture(material.bump[0], texCoords).r);
    else
        N = normalize(fs_in.Normal);

    for(int i = 0; i < numPointLights; i++)
    {
        vec3 lightDirection = normalize(pointLights[i].position - fs_in.FragPos);

        if(dot(lightDirection, N) < 0.0)
            occlusionTex = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

//PARALLAX MAPPING
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float minLayers = 8.0;
    const float maxLayers = 64.0;//32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
   
    vec2 p = viewDir.xy * material.heightScale;
    vec2 deltaTexCoords = p / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(material.height[0], currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.height[0], currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.height[0], prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;  
}
