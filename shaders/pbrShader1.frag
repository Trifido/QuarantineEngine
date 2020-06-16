#version 430 core
layout (location = 0) out vec4 FragColor;

#define NUM_TEXTURES 1
#define NR_DIR_LIGHTS 1
//const int NR_POINT_LIGHTS 2
#define NR_SPOT_LIGHTS 1

const float PI = 3.14159265359;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentViewPos;
    vec3 TangentFragPos; 
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
    int blinn;
}; 

struct PointLight {    
    vec3 position;
    vec3 diffuse;
    float constant;
    float linear;
    float quadratic;
    float far_plane;
    samplerCube shadowCubeMap;
};

//MATERIAL
uniform Material material;

//LIGHTS
uniform int numPointLights;
const int num = 1;
uniform PointLight pointLights[num];
uniform float generalAmbient;

//CAMERA POSITION
uniform vec3 viewPos;

//BLINN-PHONG LIGHT EQUATIONS
vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords); 
//PARALLAX FUNCTION
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
//SHADOW COMPUTE
float PointShadowCalculation(int idLight);

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

void main()
{
    //GET TEXTURE COORDS 
    vec2 texCoords;

    if(material.num_height > 0)
        texCoords = ParallaxMapping(fs_in.TexCoords,  normalize(fs_in.TangentViewPos - fs_in.TangentFragPos));
    else
        texCoords = fs_in.TexCoords;

    vec3 N;
    if(material.num_normal > 0)
        N = getNormalFromMap(texCoords);
    else if(material.num_bump > 0)
        N = fs_in.Normal * texture(material.bump[0], texCoords).r;
    else
        N = fs_in.Normal;
 
    //vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 V = normalize(camera_position - fs_in.FragPos);

    vec3 albedo     = pow(texture(material.diffuse[0], texCoords).rgb, vec3(2.2));

    float roughness = texture(material.roughness[0], texCoords).r;
    float ao        = texture(material.ao[0], texCoords).r;

    if(texture(material.diffuse[0], texCoords).a < 0.1)
        discard; 

    //COMPUTE LIGHT
    // reflectance equation
    vec3 Lo = vec3(0.0);
    vec3 result = texture(material.diffuse[0], texCoords).rgb * generalAmbient;

    for(int i = 0; i < numPointLights; i++)
        Lo += CalcPointLight(pointLights[i], i, N, V, texCoords);

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
     
    FragColor = vec4(color, 1.0); 
}

vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
{
    vec3 albedo     = pow(texture(material.diffuse[0], texCoords).rgb, vec3(2.2));
    float metallic;
    if(material.num_metallic > 0)
        metallic = texture(material.metallic[0], texCoords).r;
    else
        metallic = 0.0;
    float roughness = texture(material.roughness[0], texCoords).r;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // calculate per-light radiance
    vec3 L = normalize(light.position - fs_in.FragPos);
    vec3 H = normalize(viewDir + L);
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = (1.0 * light.linear) / (distance * distance);
    vec3 radiance = light.diffuse * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);   
    float G   = GeometrySmith(normal, viewDir, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);
           
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    vec3 specular = nominator / denominator;
        
    // kS is equal to Fresnel
    vec3 kS = F;

    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;

    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  

    // scale light by NdotL
    float NdotL = max(dot(normal, L), 0.0);        

    float shadow = PointShadowCalculation(idLight);

    // add to outgoing radiance Lo
    return (kD * albedo * (1.0 - shadow) / PI + (specular * (1.0 - shadow))) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
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

float PointShadowCalculation(int idLight)
{
    float shadow  = 0.0;
    float bias   = 0.000f;
    int samples = 20;
    float viewDistance;
    vec3 fragToLight;

    //viewDistance = length(viewPos - fs_in.FragPos);
    viewDistance = length(camera_position - fs_in.FragPos);
    fragToLight = fs_in.FragPos - pointLights[idLight].position;

    float diskRadius = (1.0 + (viewDistance / pointLights[idLight].far_plane)) / 25.0; 
    float currentDepth = length(fragToLight);

    float closestDepth = texture(pointLights[idLight].shadowCubeMap, fragToLight).r;

    closestDepth *= pointLights[idLight].far_plane;   // Undo mapping [0;1]

    shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0; 

    return shadow;
}
