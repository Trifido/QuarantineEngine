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
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

struct Material {
    int num_diffuse;
    int num_specular;
    int num_emissive;
    int num_normal;
    int num_height;
    int num_bump;

    sampler2D diffuse[NUM_TEXTURES];
    sampler2D specular[NUM_TEXTURES];
    sampler2D emissive[NUM_TEXTURES];
    sampler2D height[NUM_TEXTURES];
    sampler2D normal[NUM_TEXTURES];
    sampler2D bump[NUM_TEXTURES];

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
    //samplerCube shadowCubeMap;
    float far_plane;
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

//MATERIAL
uniform Material material;

//LIGHTS
uniform int numSpotLights;
uniform int numPointLights;
uniform int numDirLights;
uniform int numFPSLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform float generalAmbient;

//CAMERA POSITION
uniform vec3 viewPos;

//DIRECTIONAL, SPOT & POINT SHADOW CAST EQUATIONS
float PointShadowCalculation(int idLight);
float DirShadowCalculation(int idLight);

//PARALLAX MAPPING

//BLINN-PHONG LIGHT EQUATIONS
vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords);
vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords);

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
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    //GET TEXTURE COORDS
    vec2 texCoords = fs_in.TexCoords;  

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
    else if(material.num_bump > 0)
    {
        normal = normal * texture(material.bump[0], texCoords).r;
        normal = normalize(normal * 2.0 - 1.0);
    }

    //COMPUTE LIGHT
    vec3 result = texture(material.diffuse[0], texCoords).rgb * generalAmbient;
    vec3 resultPoint = vec3(0.0);
    vec3 resultDir = vec3(0.0);
    vec3 resultSpot = vec3(0.0);
    vec3 resultFPS = vec3(0.0);

    // phase 1: Directional lighting 
    for(int i = 0; i < numDirLights; i++)
        resultPoint = CalcDirLight(dirLights[i], i, normal, viewDir, texCoords);

    // phase 2: Point lights
    for(int i = 0; i < numPointLights; i++)
        resultPoint = CalcPointLight(pointLights[i], i, normal, viewDir, texCoords);

    // phase 3: Spot lights

    result *= (resultPoint + resultDir + resultSpot);// + resultFPS);
    FragColor = vec4(result, 1.0);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); 
}

vec3 CalcPointLight(PointLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir = normalize(pointLights[idLight].position - fs_in.FragPos);

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

    float distance= length(light.position - fs_in.FragPos);

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

vec3 CalcDirLight(DirLight light, int idLight, vec3 normal, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);

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
    vec3 diffuse  = light.diffuse  * diff * resultDiffuse;
    vec3 specular = light.specular * spec * resultSpecular;
    vec3 emissive = resultEmissive;

    return (resultDiffuse * generalAmbient + (1.0 - shadow) * (diffuse + specular + emissive));
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
        float closestDepth = 0.0;//texture(pointLights[idLight].shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
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
