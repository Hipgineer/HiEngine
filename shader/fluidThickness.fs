#version 330 core

void main() {
    // Calculate Normal of the fragment Points
    float x =   (gl_PointCoord.x * 2.0 - 1.0);
    float y = - (gl_PointCoord.y * 2.0 - 1.0);
    float z = sqrt(1.0 - (pow(x, 2.0) + pow(y, 2.0)));
    vec4 pixelNorm = vec4(x, y, z, 0.0); // positions of fragment
    float mag = dot(pixelNorm.xy, pixelNorm.xy);
    if(mag > 1.0) discard;

    pixelNorm = normalize(pixelNorm);

	gl_FragColor = vec4(pixelNorm.z*0.05, 0.0, 0.0, 1.0);
}