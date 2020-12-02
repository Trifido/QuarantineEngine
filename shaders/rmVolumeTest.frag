#version 330 core

#define NUM_TEXTURES 1

struct ray_t {
	vec3 origin;
	vec3 direction;
};

struct Material {
    int num_perlin;
    sampler2D perlin[NUM_TEXTURES];
    int num_worley;
    sampler2D worley[NUM_TEXTURES];
};

struct AABB {
	vec3 top;
	vec3 bottom;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
} fs_in;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec4 FragColor;

uniform float time;
uniform AABB collider;
uniform vec2 viewport_size;
uniform float aspect_ratio;
uniform float focal_length;
uniform float step_length;

#define NUM_STEPS 100
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;

//CAMERA POSITION
uniform vec3 viewPos;

//MATERIAL PROPERTIES
uniform Material material;

void ray_box_intersection(ray_t ray, out float t_0, out float t_1)
{
    vec3 direction_inv = 1.0 / ray.direction;
    vec3 t_top = direction_inv * (collider.top - ray.origin);
    vec3 t_bottom = direction_inv * (collider.bottom - ray.origin);
    vec3 t_min = min(t_top, t_bottom);
    vec2 t = max(t_min.xx, t_min.yz);
    t_0 = max(0.0, max(t.x, t.y));
    vec3 t_max = max(t_top, t_bottom);
    t = min(t_max.xx, t_max.yz);
    t_1 = min(t.x, t.y);
}

float sdSphere( vec3 p, vec3 center, float s )
{
    return length(p - center) - s;
}

float map_the_world(vec3 p)
{
    float displacement = sin(5.0 * p.x + time) * sin(5.0 * p.y + time) * sin(5.0 * p.z + time) * 0.25;
    float sphere_0 = sdSphere(p, vec3(0.0), 0.3);
    return texture(material.worley[0], fs_in.TexCoords).r;
    return sphere_0 ;//+ displacement;
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

vec3 rayMarching(ray_t ray)
{
    vec3 result = vec3(0.0);
    float total_distance_traveled = 0.0;

    for(int i = 0; i < NUM_STEPS; ++i)
    {
        vec3 current_position = ray.origin + total_distance_traveled * ray.direction;
        float dist_compute = map_the_world(current_position);

        if(dist_compute < EPSILON)
        {
            vec3 normal = calculate_normal(current_position);
            vec3 light_position  = vec3(2.0, -5.0, 3.0);
            vec3 dir_to_light = normalize(current_position - light_position);

            float diffuse = max(0.0, dot(normal, dir_to_light));
            return vec3(1.0, 0.0, 0.0) * diffuse;
        }

        total_distance_traveled += dist_compute;

        if(total_distance_traveled > MAX_DIST)
        {
            break;
        }
    }

    return result;
}

void main()
{
    vec3 worldPosition = viewPos;
    vec3 ray_direction;
    ray_direction.xy = 2.0 * gl_FragCoord.xy / viewport_size - 1.0;
    ray_direction.x *= aspect_ratio;
    ray_direction.z = -focal_length;
    ray_direction = (vec4(ray_direction, 0) * view).xyz;

    ray_t casting_ray;
    casting_ray.origin = worldPosition;
    casting_ray.direction = ray_direction;

    float t_0, t_1;
    ray_box_intersection(casting_ray, t_0, t_1);

    vec3 ray_start = (worldPosition + ray_direction * t_0 - collider.bottom) / (collider.top - collider.bottom);
    vec3 ray_stop = (worldPosition + ray_direction * t_1 - collider.bottom) / (collider.top - collider.bottom);

    vec3 ray = ray_stop - ray_start;
    float ray_length = length(ray);
    //vec3 step_vector = step_length * ray / ray_length;

    ray_t final_ray;
    final_ray.origin = ray_start;
    final_ray.direction = ray;

    //while (ray_length > 0)
    //{
    //    ray_length -= step_length;
    //    position += step_vector;
    //}

    vec3 finalColor = rayMarching(final_ray);

    FragColor = vec4(finalColor, 1.0);
}
