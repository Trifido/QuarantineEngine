#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
    
uniform vec3 viewPos;
uniform Light light;  
uniform Material material;

void main()
{
    // ambient 
    vec3 ambient = material.ambient * light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;

    //specular 
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = (spec * material.specular) * light.specular;  
    
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
} 
