#version 330 core

uniform mat4 projTransform;
uniform float pointRadius;

in vec3 eyeSpacePos;

void main() {
    // Calculate Normal of the fragment Points
    float x =   (gl_PointCoord.x * 2.0 - 1.0);
    float y = - (gl_PointCoord.y * 2.0 - 1.0);
    float z = sqrt(1.0 - (pow(x, 2.0) + pow(y, 2.0)));
    vec4 pixelNorm = vec4(x, y, z, 0.0); // positions of fragment
    float mag = dot(pixelNorm.xy, pixelNorm.xy);
    if(mag > 1.0) discard;

    // pixelNorm = transpose(pointTransform)*normalize(pixelNorm);
    pixelNorm = normalize(pixelNorm);

    vec4 pixelPos = vec4(eyeSpacePos + pixelNorm.xyz*pointRadius, 1.0);
    vec4 clipSpacePos = projTransform * pixelPos;
    gl_FragDepth = clipSpacePos.z / clipSpacePos.w;
    // gl_FragColor = vec4(vec3(clipSpacePos.z / clipSpacePos.w), 1.0);
}