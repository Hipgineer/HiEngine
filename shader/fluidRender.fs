#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D texDepth;

void main() {
    fragColor = texture(texDepth, texCoord);
}