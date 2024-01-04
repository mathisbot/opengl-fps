#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D brickwallTexture;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform uvec2 windowSize;
uniform float pointerRadius;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
    float quadratic;
};
uniform Light light;

void main()
{
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    // ambient
    vec3 ambient = material.ambient * light.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec4 result = vec4((ambient+diffuse+specular) * objectColor * lightColor * attenuation, 1.0);
    vec4 color = texture(brickwallTexture, TexCoords) * result;

    float distanceCenter = length(gl_FragCoord.xy-windowSize/2);

    if (distanceCenter <= pointerRadius)
    {
        FragColor = vec4(1-color.rgb, color.a);
    }
    else
    {
        FragColor = color;
    }

}