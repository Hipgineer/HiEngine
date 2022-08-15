#version 330 core
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColor;

uniform vec3 cameraPos;
uniform mat4 transform; // MVP matrix
uniform mat4 projTransform;
uniform mat4 viewTransform;
uniform float pointRadius;
uniform float pointScale;
uniform float colorMax;
uniform float colorMin;

out float vrtDepth;
out vec3  vrtPos;
out vec3  vrtColor;
out vec3  eyeSpacePos;

void main() {
	vrtPos = aPos;
	vec4 viewPosition = viewTransform * vec4(aPos, 1.0);
	eyeSpacePos = viewPosition.xyz;
    gl_Position = transform * vec4(aPos, 1.0);
	gl_PointSize = - (pointScale * pointRadius / viewPosition.z);

	// density visualization
	vrtDepth = viewPosition.z / viewPosition.w; //viewPosition.z; // length(vrtPos - cameraPos);
	vrtColor = mix(vec3(0.7,0.7,0.7), vec3(1.0,0.0,0.0),  (aColor-colorMin)/(colorMax-colorMin));
}