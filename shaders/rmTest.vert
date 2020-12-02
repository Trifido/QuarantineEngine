#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec4 near_4;   
out vec4 far_4;
out mat4 viewproj;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
    mat4 VPMatrix = projection * view;
    viewproj = VPMatrix;
    mat4 inverse_matrix = inverse(VPMatrix);

    //get 2D projection of this vertex in normalized device coordinates
    vec2 pos = gl_Position.xy/gl_Position.w;
   
    //compute ray's start and end as inversion of this coordinates
    //in near and far clip planes
    near_4 = inverse_matrix * (vec4(pos, -1.0, 1.0));       
    far_4 = inverse_matrix * (vec4(pos, 1.0, 1.0));

} 
