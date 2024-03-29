#version 460 core
out vec4 FragColor;

uniform uvec2 windowSize;
uniform float pointerRadius;
uniform vec3 lightColor;

void main()
{
    // Drawing crosshair
    float distance = length(gl_FragCoord.xy-windowSize/2);
    if (distance <= pointerRadius) FragColor = vec4(1-lightColor, 1.0);
    else FragColor = vec4(lightColor, 1.0);
}