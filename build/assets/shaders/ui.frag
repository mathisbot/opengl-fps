#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
    sampler2D normalMap;
};
uniform Material material;

void main()
{
    FragColor = vec4(texture(material.diffuseMap, TexCoords).rgb, 1.0);
}