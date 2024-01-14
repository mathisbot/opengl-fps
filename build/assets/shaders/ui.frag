#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture2D;

void main()
{
    FragColor = vec4(texture(texture2D, TexCoords).rgb, 1.0);
}