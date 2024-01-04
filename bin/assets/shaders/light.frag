#version 460 core
out vec4 FragColor;

uniform uvec2 windowSize;
uniform float pointerRadius;
uniform vec3 lightColor;

void main()
{
    vec4 color = vec4(lightColor, 1.0);
    float distance = length(gl_FragCoord.xy-windowSize/2);

    if (distance <= pointerRadius)
    {
        FragColor = vec4(1-color.rgb, color.a);
    }
    else
    {
        FragColor = color;
    }
}