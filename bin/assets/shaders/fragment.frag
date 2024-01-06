#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform samplerCube depthCubemap;
uniform float farPlane;

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


float computeShadow(vec3 fragPos)
{
    vec3 fragToLight = fragPos - pointLights[0].position;
    float currentDepth = length(fragToLight);

    int samples = 20;
    vec3 sampleOffsetDirections[20] = vec3[]
    (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );   

    float shadow = 0.0;
    float bias = 0.15;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
    for (int i=0; i<samples; i++)
    {
        float closestDepth = texture(depthCubemap, fragToLight + sampleOffsetDirections[i]*diskRadius).r * farPlane;
        shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    float shadow = 0.0;
    // Only compute shadow for white light
    if (light.color == vec3(1.0)) shadow = computeShadow(fragPos);

    return (ambient + (1-shadow)*(diffuse+specular)) * light.color * attenuation;
}

void main()
{
    vec3 outputColor = vec3(0.0);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    for (int i = 0; i < NR_POINT_LIGHTS; i++) outputColor += computePointLight(pointLights[i], norm, FragPos, viewDir);

    vec4 result = texture(textureSampler, TexCoords) * vec4(outputColor, 1.0);

    // Draw pointer circle
    float distanceCenter = length(gl_FragCoord.xy-windowSize/2);
    if (distanceCenter <= pointerRadius) FragColor = vec4(1-result.rgb, result.a);
    else FragColor = result;
}