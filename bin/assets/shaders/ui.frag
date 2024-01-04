#version 460 core
out vec4 FragColor;

uniform uvec2 windowSize;
uniform float pointerRadius;

void main()
{
    vec2 uv = (gl_FragCoord.xy / windowSize)*2.0 - 1.0;
    float distance = length(uv);

    if (distance < pointerRadius)
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        discard;
    }
}