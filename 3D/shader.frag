#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 color;
    vec3 intensity;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
  

uniform vec3 viewPos; 

uniform sampler2D uDiffMap1;

uniform int fragType; 

uniform Material material;
uniform DirLight dirLight;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    if (fragType == 0) // ocean = 0
        FragColor = vec4(result, 1.0);
    else if (fragType == 1) // clouds = 1
        FragColor = vec4(1.0, 1.0, 1.0, 0.5);
    else if (fragType == 2) // island = 2
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    else if (fragType == 3) // fire = 3
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if (fragType == 4) // palm tree = 4
        FragColor = vec4(result, 1.0);
    else if (fragType == 5) // sharks = 5
        FragColor = vec4(result, 1.0);

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)) * light.color * light.intensity;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)) * light.color * light.intensity;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)) * light.color * light.intensity;
    return (ambient + diffuse + specular);
}