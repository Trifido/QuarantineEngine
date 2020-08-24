#version 430 core

#define NR_DIR_LIGHTS 1
#define NR_POINT_LIGHTS 1
#define NR_SPOT_LIGHTS 1

layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords; 
} fs_in;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gSpecular;
uniform sampler2D gEmissive;
//uniform sampler2D ssao;
//uniform sampler2D gLightVolumeValue;

uniform vec3 viewPos;

struct DirLight {
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //sampler2D shadowMap;
};  

struct PointLight {    
    vec3 position;
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float far_plane;
    float radius;
    
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
    //sampler2D shadowMap;
};

//LIGHTS
uniform SpotLight fpsSpotLight;
uniform int numSpotLights;
uniform int numPointLights;
uniform int numDirLights;
uniform int numFPSLights;
//uniform DirLight dirLights[NR_DIR_LIGHTS];
//uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform float generalAmbient;
uniform float gamma; 
uniform float exposure;

uniform samplerCube shadowCubeMap;

//BLINN-PHONG LIGHT EQUATIONS
vec3 CalcPointLight(int idLight, vec3 FragPos, vec3 normal, vec3 viewDir);
//CAST SHADOWS
float PointShadowCalculation(int idLight, vec3 FragPos);

void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, fs_in.TexCoords).rgb;

    vec3 fragToLight = normalize(FragPos - pointLights[0].position);
    float closestDepth = texture(shadowCubeMap, fragToLight).r;

    vec3 Diffuse = texture(gAlbedo, fs_in.TexCoords).rgb;
    vec3 Normal = texture(gNormal, fs_in.TexCoords).rgb;
    //float AmbientOcclusion = texture(ssao, fs_in.TexCoords).r;

    vec3 result = Diffuse * generalAmbient;// * AmbientOcclusion;
    vec3 resultPoint = vec3(0.0);
    vec3 viewDir  = normalize(viewPos - FragPos);

    // phase 2: Point lights
    for(int i = 0; i < numPointLights; i++)
    {
        resultPoint += CalcPointLight(i, FragPos, Normal, viewDir);
    }

    result *= resultPoint; 
    result = vec3(1.0) - exp(-result * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(texture(shadowCubeMap, fragToLight).rgb, 1.0);
}  

vec3 CalcPointLight(int idLight, vec3 FragPos, vec3 normal, vec3 viewDir)
{
    float distance = length(pointLights[idLight].position - FragPos);
    // - DIFFUSE
    vec4 resultDiffuse = texture(gAlbedo, fs_in.TexCoords);
    //if(pointLights[idLight].radius < distance)
    //    return (resultDiffuse.rgb * generalAmbient);

    vec3 lightDir = normalize(pointLights[idLight].position - FragPos);    

    float diff = max(dot(lightDir, normal), 0.0);

    float attenuation = 1.0 / (pointLights[idLight].constant + pointLights[idLight].linear * distance + pointLights[idLight].quadratic * (distance * distance));

    //SAMPLE TEXTURES
    // - SPECULAR
    vec3 resultSpecular = texture(gSpecular, fs_in.TexCoords).rgb;
    // - EMISSIVE
    vec3 resultEmissive = texture(gEmissive, fs_in.TexCoords).rgb;

    // specular BLINNPHONG-shading OR PHONG-shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), resultDiffuse.a);

    // combine results
    float shadow = PointShadowCalculation(idLight, FragPos); 
    vec3 diffuse  = pointLights[idLight].diffuse * diff * resultDiffuse.xyz * attenuation;
    vec3 specular = pointLights[idLight].specular * spec * resultSpecular * attenuation;
    vec3 emissive = resultEmissive * 10;

    return (resultDiffuse.rgb * generalAmbient + (1.0 - shadow) * (diffuse + specular + emissive));
}

//CAST SHADOWS

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float PointShadowCalculation(int idLight, vec3 FragPos)
{
    float shadow  = 0.0;
    float bias   = 0.000f;
    int samples = 20;
    float viewDistance = length(viewPos - FragPos);
    vec3 fragToLight = FragPos - pointLights[idLight].position;

    float diskRadius = (1.0 + (viewDistance / pointLights[idLight].far_plane)) / 25.0; 
    float currentDepth = length(fragToLight);

    //for(int i = 0; i < samples; ++i)
    //{
    //    float closestDepth = texture(pointLights[idLight].shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
    //    closestDepth *= pointLights[idLight].far_plane;   // Undo mapping [0;1]
    //    if(currentDepth - bias > closestDepth)
    //        shadow += 1.0;
    //}

    //shadow /= float(samples);

    float closestDepth = 0.5;//texture(pointLights[idLight].shadowCubeMap, fragToLight).r;

    closestDepth *= pointLights[idLight].far_plane;   // Undo mapping [0;1]

    shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0; 

    return shadow;
}
