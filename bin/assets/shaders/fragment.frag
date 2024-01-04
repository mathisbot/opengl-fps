#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D textureSampler;  // Property of the material
uniform vec3 viewPos;

uniform uvec2 windowSize;
uniform float pointerRadius;

struct Material {
    // sampler2D diffuse;
    // sampler2D specular;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct PointLight {
    vec3 color;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
    float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];


vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * light.color * attenuation;
}

void main()
{
    vec3 outputColor = vec3(0.0);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    for (int i = 0; i < NR_POINT_LIGHTS; i++) outputColor += computePointLight(pointLights[i], norm, FragPos, viewDir);

    // Result pixel color
    vec4 result = texture(textureSampler, TexCoords) * vec4(outputColor, 1.0);

    // Draw pointer circle
    float distanceCenter = length(gl_FragCoord.xy-windowSize/2);
    if (distanceCenter <= pointerRadius) FragColor = vec4(1-result.rgb, result.a);
    else FragColor = result;
}