#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;
out vec4 brightColor;

uniform sampler2D tex;
uniform sampler2D texDepth;
uniform mat4 iProjTransform;
uniform mat4 iViewTransform;
uniform mat4 viewTransform;


vec4 liquidColor = vec4(0.0, 0.5, 1.0, 1.0);
vec4 backgroundColor = vec4(vec3(1.0), 1.0);

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

vec3 uvToEye(vec2 coord, float z)
{
	vec2 pos = coord * 2.0f - 1.0f;
	vec4 clipPos = vec4(pos, z, 1.0f);
	vec4 viewPos = iProjTransform * clipPos;
	return viewPos.xyz / viewPos.w;
}

void main() {

    float depth = texture(texDepth, texCoord).r;
	gl_FragDepth = depth;
	if(depth <= -1.0f || depth >= 1.0f)
	{
        // fragColor = texture(tex, texCoord);
        fragColor = backgroundColor;
		return;
	}


	// -----------------reconstruct normal----------------------------
	vec2 depthTexelSize = 1.0 / textureSize(texDepth, 0);
	// calculate eye space position.
	vec3 eyeSpacePos = uvToEye(texCoord, depth);
	// finite difference.
	vec3 ddxLeft   = eyeSpacePos - uvToEye(texCoord - vec2(depthTexelSize.x,0.0f),
					texture(texDepth, texCoord - vec2(depthTexelSize.x,0.0f)).r);
	vec3 ddxRight  = uvToEye(texCoord + vec2(depthTexelSize.x,0.0f),
					texture(texDepth, texCoord + vec2(depthTexelSize.x,0.0f)).r) - eyeSpacePos;
	vec3 ddyTop    = uvToEye(texCoord + vec2(0.0f,depthTexelSize.y),
					texture(texDepth, texCoord + vec2(0.0f,depthTexelSize.y)).r) - eyeSpacePos;
	vec3 ddyBottom = eyeSpacePos - uvToEye(texCoord - vec2(0.0f,depthTexelSize.y),
					texture(texDepth, texCoord - vec2(0.0f,depthTexelSize.y)).r);
	vec3 dx = ddxLeft;
	vec3 dy = ddyTop;
	if(abs(ddxRight.z) < abs(ddxLeft.z))
		dx = ddxRight;
	if(abs(ddyBottom.z) < abs(ddyTop.z))
		dy = ddyBottom;
	vec3 normal = normalize(cross(dx, dy));
	vec3 worldPos = (iViewTransform * vec4(eyeSpacePos, 1.0f)).xyz;

	// -----------------refracted----------------------------
	float thickness = max(texture(tex, texCoord).r, 0.3f);
	vec3 transmission = exp(-(vec3(1.0f) - liquidColor.xyz) * thickness);

	vec2 texScale = vec2(0.75, 1.0);		// ???.
	float refractScale = 1.33 * 0.025;	// index.
	refractScale *= smoothstep(0.1, 0.4, worldPos.y);
	vec2 refractCoord = texCoord + normal.xy * refractScale * texScale;
	// vec3 refractedColor = texture(backgroundTex, refractCoord).xyz * transmission;
	vec3 refractedColor = backgroundColor.xyz * transmission;
    // vec3 refractedColor = transmission;

	// -----------------Phong lighting----------------------------
	vec3 viewDir = -normalize(eyeSpacePos);
	vec3 lightDir = normalize((viewTransform * vec4(- light.direction, 0.0f)).xyz);
	vec3 halfVec = normalize(viewDir + lightDir);
	vec3 specular = vec3(light.specular * pow(max(dot(halfVec, normal), 0.0f), 400.0f));
	vec3 diffuse = liquidColor.xyz * max(dot(lightDir, normal), 0.0f) * light.diffuse * liquidColor.w;
	
	// -----------------Merge all effect----------------------------
	fragColor.rgb = diffuse + specular + refractedColor;
	fragColor.a = 1.0f;
	// gamma correction.
	// glow map.
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	brightColor = vec4(fragColor.rgb * brightness * brightness, 1.0f);


    // fragColor = vec4(normal, 1.0);
    // fragColor = texture(tex, texCoord);
}