#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
 
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;


void main() {
    vec3 ambient = material.diffuse * light.ambient;
 
    float dist = length(light.position - position);
    vec3 distPoly = vec3(1.0, dist, dist*dist);

    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.position - position) / dist;

    float theta = dot(lightDir, normalize(-light.direction));
        
    float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);
    vec3 result = ambient;

    if (intensity > 0.0 ) {
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * material.diffuse * light.diffuse;
        vec3 viewDir = normalize(viewPos - position);
        vec3 reflectDir = reflect(-lightDir, pixelNorm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = spec * material.specular * light.specular;

        result += (diffuse + specular) * intensity;
    }

    result *= attenuation;

    fragColor = vec4(result, 1.0);
}


