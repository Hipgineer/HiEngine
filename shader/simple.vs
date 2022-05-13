#version 330 core
layout (location = 0) in vec3 aPos; // attribute 0번!

void main() {
    gl_Position = vec4(aPos, 1.0);
}