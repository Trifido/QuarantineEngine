#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

#define NUM_TEXTURES 4
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4 

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
    int blinn;
};

struct DirLight {
    vec3 direction;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {    
    vec3 position;
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
};

//MATERIAL
uniform Material material;
//LIGHTS
uniform DirLight dirLight;
uniform SpotLight fpsSpotLight;
uniform int numSpotLights;
uniform int numPointLights;
uniform int numDirLights;
uniform int numFPSLights;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 viewPos; 

//PHONG LIGHT EQUATIONS
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    if(texture(material.diffuse[0], TexCoords).a < 0.1)
        discard;

    // GET NORMALS
    vec3 norm;
    if(material.num_normal > 0)
    {
        norm  = normalize(vec3(texture(material.normal[0], TexCoords)));
        norm = norm * 2.0 - 1.0;
        norm = normalize(TBN * norm); 
    }
    else
    {
        norm = normalize(Normal);
    }

    //GET VIEW DIRECTION
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = texture(material.diffuse[0], TexCoords).rgb;
    vec3 resultPoint = vec3(0.0);
    vec3 resultDir = vec3(0.0);
    vec3 resultSpot = vec3(0.0);
    vec3 resultFPS = vec3(0.0);

    // phase 1: Directional lighting
    if(numDirLights > 0)
        resultDir += CalcDirLight(dirLight, norm, viewDir);

    // phase 2: Point lights
    for(int i = 0; i < numPointLights; i++)
        resultPoint += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // phase 3: Spot lights
    for(int i = 0; i < numSpotLights; i++)
        resultSpot += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);

    //phase 4: FPS Spot Light
    if(numFPSLights > 0)
        resultFPS += CalcSpotLight(fpsSpotLight, norm, FragPos, viewDir);

    result *= (resultPoint + resultDir + resultSpot + resultFPS);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    // specular BLINNPHONG-shading
    float spec;

    if(material.blinn > 0)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);
    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], TexCoords));

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);
    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], TexCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], TexCoords));

    vec3 diffuse  = light.diffuse  * diff * resultDiffuse;
    vec3 specular = light.specular * spec * resultSpecular;
    vec3 emissive = resultEmissive;
    return (diffuse + specular + emissive);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // specular BLINNPHONG-shading OR PHONG-shading
    float spec;
    if(material.blinn > 0)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    { 
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //SAMPLE TEXTURES
    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);
    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], TexCoords));

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);
    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], TexCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], TexCoords));

    // combine results
    vec3 diffuse  = light.diffuse  * diff * resultDiffuse * attenuation;
    vec3 specular = light.specular * spec * resultSpecular  * attenuation;
    vec3 emissive = resultEmissive * attenuation;

    return (diffuse + specular + emissive);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // specular BLINNPHONG-shading
    float spec;
    if(material.blinn > 0)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results

    //SAMPLE TEXTURES
    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);
    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], TexCoords));

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);
    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], TexCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], TexCoords));

    vec3 diffuse = light.diffuse * diff * resultDiffuse * attenuation * intensity;
    vec3 specular = light.specular * spec * resultSpecular * attenuation * intensity;   
    return (diffuse + specular + resultEmissive);
}
