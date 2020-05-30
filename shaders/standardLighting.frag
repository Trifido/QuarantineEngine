#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define NUM_TEXTURES 1
#define NR_DIR_LIGHTS 2
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 2

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    vec4 FragPosDirLightSpace[NR_DIR_LIGHTS];
    vec4 FragPosPointLightSpace[NR_POINT_LIGHTS];
    vec4 FragPosSpotLightSpace[NR_SPOT_LIGHTS];

    vec3 TangentDirLightPos[NR_DIR_LIGHTS];
    vec3 TangentPointLightPos[NR_POINT_LIGHTS];
    vec3 TangentSpotLightPos[NR_SPOT_LIGHTS];

    vec3 TangentSpotLightDirection[NR_SPOT_LIGHTS];
    vec3 TangentDirLightDirection[NR_DIR_LIGHTS];

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

//MATERIAL
uniform Material material;

//LIGHTS
uniform SpotLight fpsSpotLight;
uniform int numSpotLights;
uniform int numPointLights;
uniform int numDirLights;
uniform int numFPSLights;
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform float generalAmbient;

//CAMERA POSITION
uniform vec3 viewPos;

//DIRECTIONAL, SPOT & POINT SHADOW CAST EQUATIONS
float DirShadowCalculation(int idLight);
float SpotShadowCalculation(int idLight);
float PointShadowCalculation(int idLight);

//PARALLAX MAPPING
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

//BLINN-PHONG LIGHT EQUATIONS
vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords);
vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords);
vec3 CalcSpotLight(SpotLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords);

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

void main()
{

    //-----------------------------------------------------------------------------------------------------------------------------------------

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
    vec3 result = texture(material.diffuse[0], texCoords).rgb * generalAmbient;
    vec3 resultPoint = vec3(0.0);
    vec3 resultDir = vec3(0.0);
    vec3 resultSpot = vec3(0.0);
    vec3 resultFPS = vec3(0.0);

    // phase 1: Directional lighting 
    for(int i = 0; i < numDirLights; i++)
        resultDir += CalcDirLight(dirLights[i], i, normal, viewDir, texCoords);

    // phase 2: Point lights
    for(int i = 0; i < numPointLights; i++)
        resultPoint += CalcPointLight(pointLights[i], i, normal, viewDir, texCoords);

    // phase 3: Spot lights
    for(int i = 0; i < numSpotLights; i++)
        resultSpot += CalcSpotLight(spotLights[i], i, normal, viewDir, texCoords);

    //phase 4: FPS Spot Light
    //if(numFPSLights > 0)
    //    resultFPS += CalcSpotLight(fpsSpotLight, 0, normal, viewDir, texCoords);

    result *= (resultPoint + resultDir + resultSpot);// + resultFPS);
    FragColor = vec4(result, 1.0);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); 
}


vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir;
    if(material.num_normal > 0)
        lightDir = normalize(-fs_in.TangentDirLightDirection[idLight]);
    else
        lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);

    float distance;
    if(material.num_normal > 0)
        distance = length(fs_in.TangentDirLightPos[idLight] - fs_in.TangentFragPos);
    else
        distance = length(light.position - fs_in.FragPos);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

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
        resultDiffuse = vec3(texture(material.diffuse[0], texCoords));

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
    vec3 diffuse  = light.diffuse  * diff * resultDiffuse * attenuation;
    vec3 specular = light.specular * spec * resultSpecular * attenuation;
    vec3 emissive = resultEmissive;

    return (resultDiffuse * generalAmbient + (1.0 - shadow) * (diffuse + specular + emissive));
}

vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir;
    if(material.num_normal > 0)
        lightDir = normalize(fs_in.TangentPointLightPos[idLight] - fs_in.TangentFragPos);
    else
        lightDir = normalize(pointLights[idLight].position - fs_in.FragPos);

    float diff = max(dot(lightDir, normal), 0.0);

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

    float distance;
    if(material.num_normal > 0)
        distance = length(fs_in.TangentPointLightPos[idLight] - fs_in.TangentFragPos);
    else
        distance = length(light.position - fs_in.FragPos);

    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //SAMPLE TEXTURES
    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);
    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], texCoords));

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
    vec3 diffuse  = light.diffuse * diff * resultDiffuse * attenuation;
    vec3 specular = light.specular * spec * resultSpecular * attenuation;
    vec3 emissive = resultEmissive;
    return (resultDiffuse * generalAmbient + (1.0 - shadow) * (diffuse + specular + emissive));
}

vec3 CalcSpotLight(SpotLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir;
    if(material.num_normal > 0)
        lightDir = normalize(fs_in.TangentSpotLightPos[idLight] - fs_in.TangentFragPos);
    else
        lightDir = normalize(spotLights[idLight].position - fs_in.FragPos);

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
    float distance;
    if(material.num_normal > 0)
        distance = length(fs_in.TangentSpotLightPos[idLight] - fs_in.TangentFragPos);
    else
        distance = length(light.position - fs_in.FragPos);
         
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta;
    if(material.num_normal > 0)
        theta = dot(lightDir, normalize(-fs_in.TangentSpotLightDirection[idLight]));
    else
        theta = dot(lightDir, normalize(-light.direction));

    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results

    //SAMPLE TEXTURES
    // - DIFFUSE
    vec3 resultDiffuse = vec3 (1.0, 1.0, 1.0);
    if(material.num_diffuse > 0)
        resultDiffuse = vec3(texture(material.diffuse[0], texCoords));

    // - SPECULAR
    vec3 resultSpecular = vec3 (1.0, 1.0, 1.0);
    if(material.num_specular > 0)
        resultSpecular = vec3(texture(material.specular[0], texCoords));

    // - EMISSIVE
    vec3 resultEmissive = vec3 (0.0, 0.0, 0.0);
    if(material.num_emissive > 0)
        resultEmissive = vec3(texture(material.emissive[0], texCoords));

    float shadow = SpotShadowCalculation(idLight);
    vec3 diffuse = light.diffuse * diff * resultDiffuse * attenuation * intensity;
    vec3 specular = light.specular * spec * resultSpecular * attenuation * intensity;
    vec3 emissive = resultEmissive; 
    return (resultDiffuse * generalAmbient + (1.0 - shadow) * (diffuse + specular + emissive));
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
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-spotLights[idLight].direction);

    if(dot(normal, lightDir) < 0.0)
        return 0.0;

    // perform perspective divide
    vec3 projCoords = fs_in.FragPosSpotLightSpace[idLight].xyz / fs_in.FragPosSpotLightSpace[idLight].w;
    // Projection coords in range [0,1]
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

