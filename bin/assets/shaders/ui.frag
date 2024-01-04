#version 460 core
out vec4 FragColor;

uniform uvec2 windowSize;
uniform float pointerRadius;

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    // Drawing crosshair
    float distance = length(gl_FragCoord.xy-windowSize/2);
    if (distance <= pointerRadius)
    {
        gl_FragDepth = 0.99999;
        FragColor = vec4(1-color.rgb, color.a);
    }
    else discard;
}