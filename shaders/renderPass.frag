#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
 

void main()
{
    float gamma = 2.2;

    FragColor.rgb = pow(texture(screenTexture, TexCoords).rgb, vec3(1.0/gamma));
    //FragColor = texture(screenTexture, TexCoords);
} 
