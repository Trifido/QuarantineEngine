#version 330 core
out vec4 FragColor;

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
    //Sample textures

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

    vec3 result;
    // phase 1: Directional lighting
    if(numDirLights > 0)
        result = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: Point lights
    for(int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // phase 3: Spot lights
    for(int i = 0; i < numSpotLights; i++)
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);

    //phase 4: FPS Spot Light
    if(numFPSLights > 0)
        result += CalcSpotLight(fpsSpotLight, norm, FragPos, viewDir);

    //result = texture(material.normal[0], TexCoords).rgb;
    
    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results

    //SAMPLE TEXTURES
    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);

    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], TexCoords));
    else if(material.num_diffuse > 1)
    {
        for(int i = 1; i < material.num_diffuse; i++)
        {
            vec3 auxDiff = vec3(texture(material.diffuse[i], TexCoords));
            resultDiffuse = mix(resultDiffuse, auxDiff, 0.5);
        } 
    }

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);

    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], TexCoords));
    else if(material.num_specular > 1)
    {
        for(int i = 1; i < material.num_specular; i++)
        {
            vec3 auxSpec = vec3(texture(material.specular[i], TexCoords));
            resultSpecular = mix(resultSpecular, auxSpec, 0.5);
        } 
    }

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);

    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], TexCoords));
    else if(material.num_emissive > 1)
    {
        for(int i = 1; i < material.num_emissive; i++)
        {
            vec3 auxEmi = vec3(texture(material.emissive[i], TexCoords));
            resultEmissive = mix(resultEmissive, auxEmi, 0.5);
        } 
    }

    vec3 ambient  = light.ambient  * resultDiffuse;
    vec3 diffuse  = light.diffuse  * diff * resultDiffuse;
    vec3 specular = light.specular * spec * resultSpecular;
    vec3 emissive = resultEmissive;
    return (ambient + diffuse + specular + emissive);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        //SAMPLE TEXTURES
    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);

    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], TexCoords));
    else if(material.num_diffuse > 1)
    {
        for(int i = 1; i < material.num_diffuse; i++)
        {
            vec3 auxDiff = vec3(texture(material.diffuse[i], TexCoords));
            resultDiffuse = mix(resultDiffuse, auxDiff, 0.5);
        } 
    }

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);

    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], TexCoords));
    else if(material.num_specular > 1)
    {
        for(int i = 1; i < material.num_specular; i++)
        {
            vec3 auxSpec = vec3(texture(material.specular[i], TexCoords));
            resultSpecular = mix(resultSpecular, auxSpec, 0.5);
        } 
    }

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);

    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], TexCoords));
    else if(material.num_emissive > 1)
    {
        for(int i = 1; i < material.num_emissive; i++)
        {
            vec3 auxEmi = vec3(texture(material.emissive[i], TexCoords));
            resultEmissive = mix(resultEmissive, auxEmi, 0.5);
        } 
    }

    // combine results
    vec3 ambient  = light.ambient  * resultDiffuse * attenuation;
    vec3 diffuse  = light.diffuse  * diff * resultDiffuse * attenuation;
    vec3 specular = light.specular * spec * resultSpecular * attenuation;
    vec3 emissive = resultEmissive * attenuation;

    return (ambient + diffuse + specular + emissive);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
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
    else if(material.num_diffuse > 1)
    {
        for(int i = 1; i < material.num_diffuse; i++)
        {
            vec3 auxDiff = vec3(texture(material.diffuse[i], TexCoords));
            resultDiffuse = mix(resultDiffuse, auxDiff, 0.5);
        } 
    }

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);

    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], TexCoords));
    else if(material.num_specular > 1)
    {
        for(int i = 1; i < material.num_specular; i++)
        {
            vec3 auxSpec = vec3(texture(material.specular[i], TexCoords));
            resultSpecular = mix(resultSpecular, auxSpec, 0.5);
        } 
    }

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);

    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], TexCoords));
    else if(material.num_emissive > 1)
    {
        for(int i = 1; i < material.num_emissive; i++)
        {
            vec3 auxEmi = vec3(texture(material.emissive[i], TexCoords));
            resultEmissive = mix(resultEmissive, auxEmi, 0.5);
        } 
    }


    vec3 ambient = light.ambient * resultDiffuse;
    vec3 diffuse = light.diffuse * diff * resultDiffuse;
    vec3 specular = light.specular * spec * resultSpecular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
