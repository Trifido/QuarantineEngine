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
    vec2 TexCoordsUV;
    vec4 clipSpace;
    vec3 viewVector;
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
    int blinn;

    sampler2D diffuse[NUM_TEXTURES];
    sampler2D specular[NUM_TEXTURES];
    sampler2D emissive[NUM_TEXTURES];

    float min_uv;
    float max_uv;
    float shininess;
    float bloomBrightness;

    sampler2D reflectionTexture;
    sampler2D refractionTexture;
    sampler2D distortionTexture;
    sampler2D normalTexture;
};

struct DirLight {
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float bias;
    int samples;
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
    float bias;
    int samples;
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
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float bias;
    int samples;
    float far_plane;
    samplerCube shadowCubeMap;
    bool isCastShadow;
    int shadowMode;
};

//LIGHTS
uniform int numPointLights, numDirLights, numSpotLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform float generalAmbient;

//MATERIAL
uniform Material material;

//CAMERA POSITION
uniform vec3 viewPos;

const float waveLength = 0.02;
uniform float moveFactor;
uniform vec3 waterColor;
uniform float colorFactor;
uniform float factor_refractive;

//-- BLINN-PHONG LIGHT EQUATIONS --
vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec3 albedoColor, vec2 texCoords); 
vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec3 albedoColor, vec2 texCoords);
vec3 CalcSpotLight(SpotLight light, int idLight, vec3 normal, vec3 viewDir, vec3 albedoColor, vec2 texCoords);
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

vec3 getNormalFromMap(vec2 TexCoords)
{
    vec3 tangentNormal = texture(material.normalTexture, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec2 ndc = (fs_in.clipSpace.xy/fs_in.clipSpace.w) / 2.0 + 0.5;
    vec2 ndcInvert = vec2(ndc.x, -ndc.y);
    
    vec2 distUV = (texture(material.distortionTexture, vec2(fs_in.TexCoordsUV.x + moveFactor, fs_in.TexCoordsUV.y)).rg * 2.0 - 1.0) * waveLength;
    vec2 distUV2 = (texture(material.distortionTexture, vec2(-fs_in.TexCoordsUV.x + moveFactor, fs_in.TexCoordsUV.y + moveFactor)).rg * 2.0 - 1.0) * waveLength;

    vec2 totalDistortion = distUV + distUV2;
    ndc += totalDistortion;
    ndcInvert += totalDistortion;

    ndc = clamp(ndc, 0.001, 0.999);
    ndcInvert.x = clamp(ndcInvert.x, 0.001, 0.999);
    ndcInvert.y = clamp(ndcInvert.y, -0.999, -0.001);
   
    vec4 reflectionColor = texture(material.reflectionTexture, ndcInvert);
    vec4 refractionColor = texture(material.refractionTexture, ndc);

    //NORMAL
    vec3 normal = getNormalFromMap(totalDistortion);

    vec3 view = normalize(fs_in.viewVector);
    float refractiveFactor = dot(view, vec3(0.0, 1.0, 0.0));
    refractiveFactor = pow(refractiveFactor, factor_refractive);

    vec4 reflectRefractColor = mix(reflectionColor, refractionColor, refractiveFactor);
    reflectRefractColor = mix(reflectRefractColor, vec4(waterColor, 1.0), colorFactor);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    vec4 result = reflectRefractColor * generalAmbient;

    for(int i = 0; i < numPointLights; i++)
        Lo += CalcPointLight(pointLights[i], i, normal, view, result.rgb, ndcInvert);

    for(int i = 0; i < numDirLights; i++)
        Lo += CalcDirLight(dirLights[i], i, normal, view, result.rgb, ndcInvert);

    for(int i = 0; i < numSpotLights; i++)
        Lo += CalcSpotLight(spotLights[i], i, normal, view, result.rgb, ndcInvert);

    FragColor = vec4(Lo, 1.0);

    float brightness = dot(result.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 64.0)//material.bloomBrightness
        BrightColor = result;
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); 
}

vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec3 albedoColor, vec2 texCoords)
{
    vec3 lightDir = normalize(pointLights[idLight].position - fs_in.FragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 resultDiffuse = albedoColor;

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);
    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], texCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], texCoords));

    // combine results
    float shadow = PointShadowCalculation(idLight); 
    vec3 specular = light.specular * spec * resultSpecular * attenuation;
    vec3 emissive = resultEmissive;
    return (resultDiffuse * generalAmbient + (1.0 - shadow) * (resultDiffuse + specular + emissive));
}

vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec3 albedoColor, vec2 texCoords)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // - DIFFUSE
    vec3 resultDiffuse = albedoColor;

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);
    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], texCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], texCoords));

    //SHADOW 
    float shadow = DirShadowCalculation(idLight);
    vec3 specular = light.specular * spec * resultSpecular;
    vec3 emissive = resultEmissive;

    return (resultDiffuse * generalAmbient + (1.0 - shadow) * (resultDiffuse + specular + emissive));
}

vec3 CalcSpotLight(SpotLight light, int idLight, vec3 normal, vec3 viewDir, vec3 albedoColor, vec2 texCoords)
{    // calculate per-light radiance
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = (1.0 * light.linear) / (distance * distance);

    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    float theta = dot(normalize(light.position - fs_in.FragPos), normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // - DIFFUSE
    vec3 resultDiffuse = albedoColor;

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);
    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], texCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], texCoords));

    //SHADOW 
    float shadow = DirShadowCalculation(idLight);
    vec3 specular = light.specular * spec * resultSpecular * attenuation * intensity;
    vec3 emissive = resultEmissive;

    return (resultDiffuse * generalAmbient + (1.0 - shadow) * (resultDiffuse + specular + emissive));
}

float PointShadowCalculation(int idLight)
{
    float shadow  = 0.0;
    float bias = pointLights[idLight].bias;//0.000f;
    int samples = pointLights[idLight].samples;
    float viewDistance;
    vec3 fragToLight;

    viewDistance = length(viewPos - fs_in.FragPos);
    fragToLight = fs_in.FragPos - pointLights[idLight].position;

    float diskRadius = (1.0 + (viewDistance / pointLights[idLight].far_plane)) / 25.0; 
    float currentDepth = length(fragToLight);

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointLights[idLight].shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= pointLights[idLight].far_plane;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples); 

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

    float bias = max(dirLights[idLight].bias * (1.0 - dot(normal, lightDir)), 0.000005);
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

    float bias = max(spotLights[idLight].bias * (1.0 - dot(normal, lightDir)), 0.000005);
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
