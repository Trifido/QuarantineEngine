#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define NUM_TEXTURES 1

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    vec4 FragPosPointLightSpace[1];
    vec3 TangentPointLightPos[1];

    vec3 TangentViewPos;
    vec3 TangentFragPos;
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
};  

struct PointLight {    
    vec3 position;
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    samplerCube shadowCubeMap;
    float far_plane;
};

//MATERIAL
uniform Material material;

//LIGHTS
uniform PointLight pointLights[1];

//CAMERA POSITION
uniform vec3 viewPos;

//PARALLAX MAPPING
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

//BLINN-PHONG LIGHT EQUATION
vec3 CalcPointLight(PointLight light, vec2 texCoords);


void main()
{
    //GET VIEW DIRECTION  
    vec3 viewDir;
    if(material.num_normal > 0)
        viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    else
        viewDir = normalize(viewPos - fs_in.FragPos);

    //GET TEXTURE COORDS
    vec2 texCoords = fs_in.TexCoords; 
    if(material.num_height > 0)
        texCoords = ParallaxMapping(fs_in.TexCoords,  normalize(fs_in.TangentViewPos - fs_in.TangentFragPos));

    //if(texCoords.x > material.max_uv || texCoords.y > material.max_uv || texCoords.x < material.min_uv || texCoords.y < material.min_uv)
    //    discard;
    if(texture(material.diffuse[0], texCoords).a < 0.1)
        discard;

    vec3 normal = fs_in.Normal;
    if(material.num_normal > 0)
    {
        normal = texture(material.normal[0], texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    }

    //COMPUTE LIGHT
    //Point light
    vec3 resultPoint = CalcPointLight(pointLights[0], texCoords);

    FragColor = vec4(resultPoint, 1.0);

    float brightness = dot(resultPoint, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(resultPoint, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); 
}

vec3 CalcPointLight(PointLight light, vec2 texCoords)
{
    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);
    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], texCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], texCoords));

    // combine results
    vec3 diffuse  = light.diffuse * resultDiffuse;
    vec3 emissive = resultEmissive;

    return (diffuse + emissive);
}

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

