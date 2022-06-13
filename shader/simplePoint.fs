#version 330 core

precision mediump float;

uniform vec3 cameraPos;
uniform mat4 viewTransform;

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

in vec3 vrtPos;
out vec4 fragColor;

void main() {
    // Calculate Normal of the fragment Points
    float x =   (gl_PointCoord.x * 2.0 - 1.0);
    float y = - (gl_PointCoord.y * 2.0 - 1.0);
    float z = sqrt(1.0 - (pow(x, 2.0) + pow(y, 2.0)));
    vec4 pixelNorm = vec4(x, y, z, 0.0); // positions of fragment
    float mag = dot(pixelNorm.xy, pixelNorm.xy);
    if(mag > 1.0) discard;

    // pixelNorm = transpose(pointTransform)*normalize(pixelNorm);
    pixelNorm = normalize(pixelNorm);

    // Light Interactions
    vec3 ambient = material.diffuse * light.ambient;
    vec3 result = ambient;
    
    // Intensity according to distance to object from light.
    float dist = length(light.position - vrtPos);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);

    vec3 lightDir = (light.position - vrtPos) / dist;
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);

    if (intensity > 0.0 ) {
        lightDir = normalize((transpose(inverse(viewTransform)) * vec4(lightDir, 0.0)).xyz);
        float   diff = max(dot(pixelNorm.xyz, lightDir), 0.0);
        vec3    diffuse = diff * material.diffuse * light.diffuse;
        vec3    viewDir = (transpose(inverse(viewTransform)) * vec4(normalize(cameraPos - vrtPos),0.0)).xyz;
        vec3    reflectDir = reflect(-lightDir, pixelNorm.xyz);
        float   spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3    specular = spec * material.specular * light.specular;
        result += (diffuse + specular) * intensity;
    }

    result *= attenuation;

    fragColor = vec4(result, 1.0);
}
