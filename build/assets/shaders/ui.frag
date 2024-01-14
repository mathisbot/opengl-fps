#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform uvec2 windowSize;
uniform float pointerRadius;

uniform samplerCube skybox;

void main()
{
    vec3 outputColor = texture(skybox, TexCoords).rgb;

    // Drawing crosshair
    float distance = length(gl_FragCoord.xy-windowSize/2);
    if (distance <= pointerRadius)
    {
        FragColor = vec4(1-outputColor, 1.0);
    }
    else
    {
        FragColor = vec4(outputColor, 1.0);
    }
}