#version 460 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos)/farPlane;
    gl_FragDepth = lightDistance;
}