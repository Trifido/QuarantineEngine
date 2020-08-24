#version 330 core

#define PI                    3.14159265359

// --------- SUN PARAMETERS --------------
// TODO: Need physically correct measures. Remove these magic numbers.
//
// https://en.wikipedia.org/wiki/Luminosity
//     luminosity of the Sun which is 3.86×1026 W
//
//     sun_luminosity / (sun_dist^2 * 4pi) = 13.61839144264511 * 10^2 (Watts / meter^2)
#define SunColor              vec3(1.0, 1.0, 1.0)
#define SunIntensity          13.61839144264511
#define SunDirection          normalize(vec3(0.50, -0.6, 1.0))
#define SunDiskMultiplier     10.0
#define MAGIC_RAYLEIGH        1.0
#define MAGIC_MIE             0.2


// ------------ ATMOSPHERE PARAMETERS ------------
// Unit: meters
// 149,600,000 km = (1.496 * 10^11) m
#define SUN_DISTANCE          1.496e11
// 695,510 km = (6.9551 * 10^8) m
#define SUN_RADIUS            6.9551e8
// 6360 km = (6.36 * 10^6) m
#define EARTH_RADIUS          6.36e6
#define ATMOSPHERE_RADIUS     6.42e6
#define Hr                    7.994e3
#define Hm                    1.2e3

// Unit: 1 / meters
#define BetaR                 vec3(5.8e-6, 13.5e-6, 33.1e-6)
#define BetaM                 vec3(21e-6)

struct ray_t {
	vec3 origin;
	vec3 direction;
};

struct sphere_t {
	vec3 origin;
	float radius;
};
    
struct hit_t {
	float t;
	vec3 normal;
	vec3 origin;
};

out vec4 FragColor;
in vec2 TexCoords;

in vec4 near_4;    //for computing rays in fragment shader
in vec4 far_4;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;
uniform float gamma;

#define NUM_TEXTURES 1
#define NUM_STEPS 100
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float EPSILON = 0.0001;

struct Material {
    int num_noise;
    sampler2D noise[NUM_TEXTURES];
};

uniform Material material;

//CAMERA POSITION
uniform vec3 viewPos;
uniform vec2 resolution;
uniform vec3 front;
uniform float fov;

//ATMOSPHERE
sphere_t atmosphere = sphere_t(vec3(0.0), ATMOSPHERE_RADIUS);

void intersect_sphere( ray_t ray, sphere_t sphere, hit_t hit )
{
	vec3 rc = sphere.origin - ray.origin;
	float radius2 = sphere.radius * sphere.radius;
	float tca = dot(rc, ray.direction);

	float d2 = dot(rc, rc) - tca * tca;
	if (d2 > radius2)
		return;

	float thc = sqrt(radius2 - d2);
	float t0 = tca - thc;
	float t1 = tca + thc;

	if (t0 < 0.) t0 = t1;
	if (t0 > hit.t)
		return;

	vec3 impact = ray.origin + ray.direction * t0;

	hit.t = t0;
	hit.origin = impact;
	hit.normal = (impact - sphere.origin) / sphere.radius;
}

float sdSphere( vec3 p, float s )
{
    return length(p - vec3(0.0)) - s;
}

float phaseRayleight(float cos_angleLV)
{
    return (3.0 * (1.0 + cos_angleLV * cos_angleLV)) / (16.0 * PI);
}

float phaseMie(float cos_angleLV)
{
    float g = 0.76;
    float gg = g * g;
    float cc = cos_angleLV * cos_angleLV;
    float gc = g * cos_angleLV;

    float num = 3.0 * (1.0 - gg) * (1.0 + cc);
    float denom = (8.0 * PI) * (2.0 + gg) * pow(1.0 + gg - 2.0 * gc, 1.5);

    return num / denom;
}

// Radiance of sun before hitting the atmosphere
vec3 sunImage(ray_t camera, vec3 sunDir)
{    
    float threshold = asin(SUN_RADIUS / SUN_DISTANCE);
    float angle = acos(dot(camera.direction, -sunDir));
    if(angle <= threshold * SunDiskMultiplier)
    {
        return SunIntensity * SunColor;
    }
    return vec3(0.0);
}

vec3 sceneRendered(ray_t camera, vec3 sunDir)
{
    hit_t no_hit = hit_t(float(1e8), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));
    hit_t hit = no_hit;

    intersect_sphere(camera, atmosphere, hit);
    
    vec3 AtmosphereScattering = vec3(0.0);
    bool isGround = false;
    
    const int numSteps = 64;
    const int inscatSteps = 16;
    
    float mu = dot(-sunDir, camera.direction);
    vec3 Sun = SunIntensity * SunColor;
    vec3 T = vec3(0.0);
    
    vec3 P = camera.origin;
    float seg = hit.t / float(numSteps);
    vec3 P_step = camera.direction * seg;
    
    // from eye to the outer end of atmosphere
    for(int i=0; i<numSteps; ++i)
    {
        float height = length(P) - EARTH_RADIUS;
        
        // optical depth
        T += seg * (BetaR * exp(-height / Hr));
        T += seg * (BetaM * exp(-height / Hm));
        
        // single scattering
        hit_t hit2 = no_hit;
        ray_t ray2 = ray_t(P, -sunDir);
        intersect_sphere(ray2, atmosphere, hit2);
        
        float segLight = hit2.t / float(inscatSteps);
        vec3 PL_step = ray2.direction * segLight;
        vec3 PL = P;
        
        vec3 TL = vec3(0.0);
        bool applyScattering = true;
        for(int j=0; j<inscatSteps; ++j)
        {
            float height2 = length(PL) - EARTH_RADIUS;
            if(height2 < 0.0)
            {
                applyScattering = false;
                break;
            }
            
            TL += segLight * BetaR * exp(-height2 / Hr);
        	TL += segLight * BetaM * exp(-height2 / Hm);
            
            PL += PL_step;
        }
        if(applyScattering)
        {
            TL = exp(-TL);

            vec3 SingleScattering = vec3(0.0);
            // scattering = transmittance * scattering_coefficient * phase * radiance
            SingleScattering += MAGIC_RAYLEIGH * seg * exp(-T) * (BetaR * exp(-height / Hr)) * phaseRayleight(mu) * (TL * Sun);
            SingleScattering += MAGIC_MIE * seg * exp(-T) * (BetaM * exp(-height / Hm)) * phaseMie(mu) * (TL * Sun);
            AtmosphereScattering += SingleScattering;
        }
        
        P += P_step;
    }
    
    // Just magic number
    
    T = exp(-T);
    
    // Zero scattering
	vec3 L0 = T * sunImage(camera, sunDir);
    AtmosphereScattering += L0;
    
    return L0;
}

void main()
{
    vec3 ro = near_4.xyz/near_4.w;  //ray's origi
    vec3 far3 = far_4.xyz/far_4.w;
    vec3 rd = far3 - ro;
    rd = normalize(rd);        //ray's direction

    vec3 sunDir = SunDirection;
    ray_t eye_ray;
    eye_ray.origin = ro;
    eye_ray.direction = rd;
    
    vec3 finalColor = sceneRendered(eye_ray, sunDir);
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
    
    // Output to screen
    FragColor = vec4(finalColor, 1.0);
}
