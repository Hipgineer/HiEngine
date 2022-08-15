#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;
uniform mat4 viewTransform;
uniform float pointRadius;
uniform float pointScale;

out vec3 eyeSpacePos;

void main() {
	vec4 viewPosition = viewTransform * vec4(aPos, 1.0);
	eyeSpacePos = viewPosition.xyz;

    gl_Position = transform * vec4(aPos, 1.0);
	gl_PointSize = - (pointScale * pointRadius / viewPosition.z);
}