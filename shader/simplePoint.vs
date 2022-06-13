#version 330 core
precision mediump float;

layout (location = 0) in vec3 aPos;

uniform mat4 transform; // MVP matrix
uniform mat4 viewTransform;
uniform float pointRadius;
uniform float pointScale;

out vec3 vrtPos;

void main() {
	vrtPos = aPos;
	vec4 viewPosition = viewTransform * vec4(aPos, 1.0);

    gl_Position = transform * vec4(aPos, 1.0);
	gl_PointSize = - (pointScale * pointRadius / viewPosition.z);
}