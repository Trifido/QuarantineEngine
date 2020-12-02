#version 430 core
layout (location = 0) out vec4 depthValue;

#define NUM_TEXTURES 1

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat4 viewproj;
} fs_in;

layout (std430, binding=2) buffer shader_data
{ 
  vec3 camera_position; 
};

struct Material {
    int num_diffuse;
    int num_height;

    sampler2D diffuse[NUM_TEXTURES];
    sampler2D height[NUM_TEXTURES];

    float heightScale;
    float min_uv;
    float max_uv;
};

//MATERIAL
uniform Material material;
uniform float near;
uniform float far;

//CAMERA POSITION
uniform vec3 viewPos;

//-- PARALLAX FUNCTION --
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

float LinearizeDepth(float depth)
{
    //float zc = ( fs_in.viewproj * vec4( fs_in.FragPos, 1.0 ) ).z;
    //float wc = ( fs_in.viewproj * vec4( fs_in.FragPos, 1.0 ) ).w;
    //return zc/wc;

    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    //GET TEXTURE COORDS 
    vec2 texCoords = fs_in.TexCoords;

    if(material.num_height > 0)
        texCoords = ParallaxMapping(fs_in.TexCoords,  normalize(fs_in.TangentViewPos - fs_in.TangentFragPos));

    if(texture(material.diffuse[0], texCoords).a < 0.1)
        discard;

    depthValue = vec4(LinearizeDepth(gl_FragCoord.z));
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
