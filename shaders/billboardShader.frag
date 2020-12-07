#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec2 texCurrentCoord;
    vec2 texNextCoord;
    float blend;
} fs_in;

uniform sampler2D particleTexture;

void main()
{
    vec4 currentColor = texture(particleTexture, fs_in.texCurrentCoord);
    vec4 nextColor = texture(particleTexture, fs_in.texNextCoord);

    FragColor = mix(currentColor, nextColor, fs_in.blend);

    BrightColor = FragColor;//vec4(0.0, 0.0, 0.0, 1.0); 
}
