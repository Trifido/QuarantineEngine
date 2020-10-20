#version 430 core
in vec2 TexCoords;
//in vec4 lightPosition;

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform sampler2D DiffSpecTex;
layout(binding = 1) uniform sampler2D occlusionTexture;
uniform float gamma; 
uniform float exposure;

uniform float decay;
uniform float weight;
uniform float density;

uniform vec2 lightPos;

#define NUM_SAMPLES 100

void main()
{
    vec2 texCoord = TexCoords;
    vec2 deltaTexCoord = (TexCoords - lightPos);

    // Divide by number of samples and scale by control factor.   
    deltaTexCoord *= 1.0 / NUM_SAMPLES * density;

    // Store initial sample.    
    vec3 color = texture(occlusionTexture, texCoord).rgb;
    vec3 screenColor = texelFetch(DiffSpecTex, ivec2(gl_FragCoord), 0).rgb;

    // Set up illumination decay factor.    
    float illuminationDecay = 1.0;
  
    for (int i = 0; i < NUM_SAMPLES; i++)   
    {     
        // Step sample location along ray.     
        texCoord -= deltaTexCoord;     
            
        // Retrieve sample at new location.    
        vec3 samp = texture(occlusionTexture, texCoord).rgb;  
            
        // Apply sample attenuation scale/decay factors.     
        samp *= illuminationDecay * weight;     
            
        // Accumulate combined color.     
        color += samp;     
            
        // Update exponential decay factor.     
        illuminationDecay *= decay;   
    }   
        
    // Output final color with a further scale control factor.    
    vec3 hdrColor = screenColor * color; //screenColor + 

    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
} 
