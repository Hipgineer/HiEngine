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






uniform vec3 lightDir;
uniform vec3 lightPos;
uniform float spotMin;
uniform float spotMax;
uniform int mode;

uniform sampler2DShadow shadowTex;
uniform vec2 shadowTaps[12];
uniform float pointRadius;  // point size in world space

// sample shadow map
float shadowSample()
{
	vec3 pos = vec3(gl_TexCoord[1].xyz/gl_TexCoord[1].w);
	vec3 uvw = (pos.xyz*0.5)+vec3(0.5);

	// user clip
	if (uvw.x  < 0.0 || uvw.x > 1.0)
		return 1.0;
	if (uvw.y < 0.0 || uvw.y > 1.0)
		return 1.0;
	
	float s = 0.0;
	float radius = 0.002;

	for (int i=0; i < 8; i++)
	{
		s += shadow2D(shadowTex, vec3(uvw.xy + shadowTaps[i]*radius, uvw.z)).r;
	}

	s /= 8.0;
	return s;
}

float sqr(float x) { return x*x; }

void main()
{
    // calculate normal from texture coordinates
    vec3 normal;
    normal.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(normal.xy, normal.xy);
    if (mag > 1.0) discard;   // kill pixels outside circle
   	normal.z = sqrt(1.0-mag);

	if (mode == 2)
	{
		float alpha  = normal.z*gl_TexCoord[3].w;
		gl_FragColor.xyz = gl_TexCoord[3].xyz*alpha;
		gl_FragColor.w = alpha;
		return;
	}

    // calculate lighting
	float shadow = shadowSample();
	
	vec3 lVec = normalize(gl_TexCoord[4].xyz-(lightPos));
	vec3 lPos = vec3(gl_TexCoord[1].xyz/gl_TexCoord[1].w);
	float attenuation = max(smoothstep(spotMax, spotMin, dot(lPos.xy, lPos.xy)), 0.05);

	vec3 diffuse = vec3(0.9, 0.9, 0.9);
	vec3 reflectance =  gl_TexCoord[3].xyz;
	
	vec3 Lo = diffuse*reflectance*max(0.0, sqr(-dot(gl_TexCoord[2].xyz, normal)*0.5 + 0.5))*max(0.2,shadow)*attenuation;

	gl_FragColor = vec4(pow(Lo, vec3(1.0/2.2)), 1.0);

	vec3 eyePos = gl_TexCoord[5].xyz + normal*pointRadius;//*2.0;
	vec4 ndcPos = gl_ProjectionMatrix * vec4(eyePos, 1.0);
	ndcPos.z /= ndcPos.w;
	gl_FragDepth = ndcPos.z*0.5 + 0.5;
}