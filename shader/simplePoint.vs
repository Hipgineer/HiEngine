#version 330 core
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aColor;

uniform mat4 transform; // MVP matrix
uniform mat4 viewTransform;
uniform float pointRadius;
uniform float pointScale;
uniform float colorMax;
uniform float colorMin;

out vec3 vrtPos;
out vec3 vrtColor;

void main() {
	vrtPos = aPos;
	vec4 viewPosition = viewTransform * vec4(aPos, 1.0);

    gl_Position = transform * vec4(aPos, 1.0);
	gl_PointSize = - (pointScale * pointRadius / viewPosition.z);

	// density visualization
	vrtColor = mix(vec3(1.0,0.0,0.0), vec3(0.0,0.0,1.0),  (aColor-colorMin)/(colorMax-colorMin));
}