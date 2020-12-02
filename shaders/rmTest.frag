#version 330 core

struct ray_t {
	vec3 origin;
	vec3 direction;
};

out vec4 FragColor;
in vec2 TexCoords;

in vec4 near_4;    //for computing rays in fragment shader
in vec4 far_4;
in mat4 viewproj;

uniform sampler2D screenTexture;
uniform sampler2D depthMap;
uniform bool bloom;
uniform float exposure;
uniform float gamma;
uniform float time;

uniform float near;
uniform float far;

#define NUM_STEPS 100
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;

//CAMERA POSITION
uniform vec3 viewPos;
uniform vec2 resolution;
uniform vec3 front;
uniform float fov;

float sdSphere( vec3 p, vec3 center, float s )
{
    return length(p - center) - s;
}

float map_the_world(vec3 p)
{
    float displacement = sin(5.0 * p.x + time) * sin(5.0 * p.y + time) * sin(5.0 * p.z + time) * 0.25;
    float sphere_0 = sdSphere(p, vec3(0.0), 1.0);

    return sphere_0;// + displacement;
}

vec3 calculate_normal(vec3 p)
{
    const vec3 small_step = vec3(0.001, 0.0, 0.0);

    float gradient_x = map_the_world(p + small_step.xyy) - map_the_world(p - small_step.xyy);
    float gradient_y = map_the_world(p + small_step.yxy) - map_the_world(p - small_step.yxy);
    float gradient_z = map_the_world(p + small_step.yyx) - map_the_world(p - small_step.yyx);

    vec3 normal = vec3(gradient_x, gradient_y, gradient_z);

    return normalize(normal);
}

float linearizeDepth(vec3 currentPos)
{
    //float zc = ( viewproj * vec4( currentPos, 1.0 ) ).z;
    //float wc = ( viewproj * vec4( currentPos, 1.0 ) ).w;
    //return zc/wc;

    float depth = ( viewproj * vec4( currentPos, 1.0 ) ).z;
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

vec3 rayMarching(ray_t ray, vec3 fwd)
{
    float sceneDepth = texture(depthMap, TexCoords).r;
    vec3 result = texture(screenTexture, TexCoords).rgb;
    float total_distance_traveled = 0.0;

    for(int i = 0; i < NUM_STEPS; ++i)
    {
        vec3 current_position = ray.origin + total_distance_traveled * ray.direction;
        float dist_compute = map_the_world(current_position);

        if(dist_compute < EPSILON)
        {
            float lineDepth = linearizeDepth(current_position);
            if(lineDepth > sceneDepth){
                //return vec3(0.0, 1.0, 1.0);
                break;
            }
            //else
            //{
            //    return vec3(1.0, 0.0, 0.0);
            //}

            vec3 normal = calculate_normal(current_position);
            vec3 light_position  = vec3(2.0, -5.0, 3.0);
            vec3 dir_to_light = normalize(current_position - light_position);

            float diffuse = max(0.0, dot(normal, dir_to_light));
            //return vec3(lineDepth);
           return vec3(1.0, 0.0, 0.0) * diffuse;
        }

        total_distance_traveled += dist_compute;

        if(total_distance_traveled > MAX_DIST)
        {
            break;
        }
    }
    //return vec3(sceneDepth); 
    return result;
}

void main()
{
    vec3 ro = near_4.xyz/near_4.w;  //ray's origi
    vec3 far3 = far_4.xyz/far_4.w;
    vec3 rd = far3 - ro;
    rd = normalize(rd);        //ray's direction

    ray_t eye_ray;
    eye_ray.origin = ro;
    eye_ray.direction = rd;

    vec3 fwd = normalize(front - ro);
	//vec3 up = vec3(0, 1, 0);
	//vec3 right = cross(up, fwd);
	//up = cross(fwd, right);

    //eye_ray.direction = normalize(fwd + up * rd.y - right * rd.x);
    
    vec3 finalColor = rayMarching(eye_ray, fwd);

    //finalColor = pow(eye_ray.direction, vec3(1.0 / 2.2));
    // Output to screen
    FragColor = vec4(finalColor, 1.0);
    //vec3 depthColor = vec3(texture(depthMap, TexCoords).r);
    //FragColor = vec4(depthColor, 1.0);
}
