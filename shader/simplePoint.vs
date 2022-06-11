#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform; // MVP matrix
uniform mat4 modelTransform;
uniform float pointRadius;
uniform float pointScale;

out vec4 vertexColor;

void main() {
	vec4 viewPos = modelTransform * vec4(aPos, 1.0);
    gl_Position = transform * vec4(aPos, 1.0);
	// gl_PointSize = pointRadius / gl_Position.z;
	// gl_PointSize = - pointScale * (pointRadius / viewPos.z);
	gl_PointSize = - (pointScale * pointRadius / viewPos.z);
	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}