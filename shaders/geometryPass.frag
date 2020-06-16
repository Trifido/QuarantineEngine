#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gSpecular;
layout (location = 4) out vec4 gEmissive;

#define NUM_TEXTURES 1
#define NR_POINT_LIGHTS 4

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    vec4 FragPosPointLightSpace[NR_POINT_LIGHTS];
    vec3 TangentPointLightPos[NR_POINT_LIGHTS];

    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat3 TBN;
} fs_in;

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

    float heightScale;
    float min_uv;
    float max_uv;
    float shininess;
    int blinn;
    int isBoundingLight;
};

//MATERIAL
uniform Material material;

//PARALLAX FUNCTION
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
    gPosition = vec4(fs_in.FragPos, 1.0);
    vec2 texCoords = fs_in.TexCoords;

    if(material.num_height > 0)
    {
        texCoords = ParallaxMapping(fs_in.TexCoords,  normalize(fs_in.TangentViewPos - fs_in.TangentFragPos));
    }

    gNormal = normalize(fs_in.Normal); 
    if(material.num_normal > 0)
    {
        gNormal = texture(material.normal[0], texCoords).rgb;
        gNormal = gNormal * 2.0 - 1.0;  // this normal is in tangent space
        gNormal = normalize(fs_in.TBN * gNormal); 
    }

    if(material.num_diffuse > 0)
    {
        gAlbedo = texture(material.diffuse[0], texCoords);
        gAlbedo.a = material.shininess;
    }
    else
        gAlbedo = vec4(0.0);

    if(material.num_specular > 0)
        gSpecular = texture(material.specular[0], texCoords);
    else
        gSpecular = vec4(1.0); 

    if(material.num_emissive > 0)
        gEmissive = texture(material.emissive[0], texCoords);
    else
        gEmissive = vec4(0.0);
}

//PARALLAX MAPPING
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
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
