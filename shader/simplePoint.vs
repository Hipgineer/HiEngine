#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 normal;
out vec2 texCoord;
out vec3 position;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    normal   = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz; // 나ㅈㅜㅇ에는 이걸 uniform으로 받아오는게 빠름
    texCoord = aTexCoord;
    position = (modelTransform * vec4(aPos, 1.0)).xyz; 
}

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 gl_ModelViewProjectionMatrix; // MVP matrix
uniform mat4 gl_ModelViewMatrix;

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels

uniform mat4 lightTransform; 
uniform vec3 lightDir;
uniform vec3 lightDirView;

uniform vec4 colors[8];

uniform vec4 transmission;
uniform int mode;

//in int density;
in float density;
in int phase;
in vec4 velocity;

void main()
{
    // calculate window-space point size
	vec4 viewPos = gl_ModelViewMatrix*vec4(aPos, 1.0);

	gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos, 1.0);
	gl_PointSize = -pointScale * (pointRadius / viewPos.z);

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = lightTransform*vec4(aPos-lightDir*pointRadius*2.0, 1.0);
	gl_TexCoord[2] = gl_ModelViewMatrix*vec4(lightDir, 0.0);
    gl_TexCoord[3].xyz = mix(vec3(0.1), vec3(1.0), 0.1);
	gl_TexCoord[4].xyz = aPos;
	gl_TexCoord[5].xyz = viewPos.xyz;
}