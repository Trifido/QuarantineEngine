#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

//-- BLINN-PHONG LIGHT EQUATIONS --
vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords); 
vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords);
vec3 CalcSpotLight(SpotLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords);

//-- PARALLAX FUNCTION --
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

//-- SHADOW COMPUTE --
float PointShadowCalculation(int idLight);
float DirShadowCalculation(int idLight);
float SpotShadowCalculation(int idLight);

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
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
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
        N = normalize(fs_in.Normal * texture(material.bump[0], texCoords).r);
    else
        N = normalize(fs_in.Normal);

    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 R = reflect(-V, N);

    vec3 albedo     = pow(texture(material.diffuse[0], texCoords).rgb, vec3(2.2));

    float roughness = texture(material.roughness[0], texCoords).r;
    float ao        = texture(material.ao[0], texCoords).r;

    //if(texture(material.diffuse[0], texCoords).a < 0.1)
    //    discard; 

    //COMPUTE LIGHT
    // reflectance equation
    vec3 Lo = vec3(0.0);
    vec3 result = texture(material.diffuse[0], texCoords).rgb;// * generalAmbient;

    for(int i = 0; i < numPointLights; i++)
        Lo += CalcPointLight(pointLights[i], i, N, V, texCoords);

    for(int i = 0; i < numDirLights; i++)
        Lo += CalcDirLight(dirLights[i], i, N, V, texCoords);

    for(int i = 0; i < numSpotLights; i++)
        Lo += CalcSpotLight(spotLights[i], i, N, V, texCoords);

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).

    // ambient lighting (we now use IBL as the ambient term)
    float metallic;
    if(material.num_metallic > 0)
        metallic = texture(material.metallic[0], texCoords).r;
    else
        metallic = 0.0;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness); 
    vec3 kS = F; 
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;
    FragColor = vec4(color, 1.0);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 64.0)//material.bloomBrightness
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); 
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

vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
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
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);

    vec3 radiance = light.diffuse;

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

    float shadow = DirShadowCalculation(idLight);

    // add to outgoing radiance Lo
    return (kD * albedo * (1.0 - shadow) / PI + (specular * (1.0 - shadow))) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec3 CalcSpotLight(SpotLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
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
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = (1.0 * light.linear) / (distance * distance);

    float theta = dot(normalize(light.position - fs_in.FragPos), normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 radiance = light.diffuse * attenuation * intensity;

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

    float shadow = SpotShadowCalculation(idLight);

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

    viewDistance = length(viewPos - fs_in.FragPos);
    fragToLight = fs_in.FragPos - pointLights[idLight].position;

    float diskRadius = (1.0 + (viewDistance / pointLights[idLight].far_plane)) / 25.0; 
    float currentDepth = length(fragToLight);

    float closestDepth = texture(pointLights[idLight].shadowCubeMap, fragToLight).r;

    closestDepth *= pointLights[idLight].far_plane;   // Undo mapping [0;1]

    shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0; 

    return shadow;
}

float DirShadowCalculation(int idLight)
{
    vec3 normal, lightDir;

    normal = normalize(fs_in.Normal);
    lightDir = normalize(-dirLights[idLight].direction);

    if(dot(normal, lightDir) < 0.0)
        return 0.0;
         
    // Projection coords in range [0,1]
    vec3 projCoords = fs_in.FragPosDirLightSpace[idLight].xyz / fs_in.FragPosDirLightSpace[idLight].w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(dirLights[idLight].shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z ;

    float bias = max(0.000005 * (1.0 - dot(normal, lightDir)), 0.000005);
    float shadow = 0.0;

    if(projCoords.z < 1.0)
    {
        vec2 texelSize = 1.0 / textureSize(dirLights[idLight].shadowMap, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(dirLights[idLight].shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth + bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
    }

    return shadow;
}

float SpotShadowCalculation(int idLight)
{
    vec3 normal, lightDir;

    normal = normalize(fs_in.Normal);
    lightDir = normalize(-spotLights[idLight].direction);

    if(dot(normal, lightDir) < 0.0)
        return 0.0;
         
    // Projection coords in range [0,1]
    vec3 projCoords = fs_in.FragPosSpotLightSpace[idLight].xyz / fs_in.FragPosSpotLightSpace[idLight].w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(spotLights[idLight].shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z ;

    float bias = max(0.000005 * (1.0 - dot(normal, lightDir)), 0.000005);
    float shadow = 0.0;

    if(projCoords.z < 1.0)
    {
        vec2 texelSize = 1.0 / textureSize(spotLights[idLight].shadowMap, 0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(spotLights[idLight].shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth + bias > pcfDepth ? 1.0 : 0.0;        
            }    
        }
        shadow /= 9.0;
    }

    return shadow;
}
