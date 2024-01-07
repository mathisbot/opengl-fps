#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform float farPlane;

uniform uvec2 windowSize;
uniform float pointerRadius;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;
uniform sampler2D textureSampler;

struct PointLight {
    vec3 color;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear;
    float quadratic;
    samplerCube depthCubemap;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

const vec3 sampleOffsetDirections[20] = vec3[]
(
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 


float computeShadow(vec3 lightPos, samplerCube depthCubemap, vec3 lightDir, vec3 normal, float diskRadius)
{
    vec3 fragToLight = FragPos - lightPos;


    float shadow = 0.0;
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float bias = 0.005;
    float currentDepth = (length(fragToLight)-bias) / farPlane;
    for (int i=0; i<20; i++)
    {
        float closestDepth = texture(depthCubemap, fragToLight + sampleOffsetDirections[i]*diskRadius).r;
        // shadow += currentDepth > closestDepth ? 1.0 : 0.0;
        shadow += currentDepth > closestDepth ? 0.05 : 0.0;
    }
    // shadow /= float(samples);

    return shadow;
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 viewDir, float diskRadius)
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular;

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    float shadow = computeShadow(light.position, light.depthCubemap, lightDir, normal, diskRadius);

    return (ambient + (1-shadow)*(diffuse+specular)) * light.color * attenuation;
}

void main()
{
    vec3 outputColor = vec3(0.0);

    vec3 norm = normalize(Normal);
    vec3 FragToView = viewPos - FragPos;
    vec3 viewDir = normalize(FragToView);
    float diskRadius = (1.0 + (length(FragToView) / farPlane)) / 25.0;
    for (int i = 0; i < NR_POINT_LIGHTS; i++) outputColor += computePointLight(pointLights[i], norm, viewDir, diskRadius);

    vec4 result = texture(textureSampler, TexCoords) * vec4(outputColor, 1.0);

    // Draw pointer circle
    float distanceCenter = length(gl_FragCoord.xy-windowSize/2);
    if (distanceCenter <= pointerRadius) FragColor = vec4(1-result.rgb, 1.0);
    else FragColor = result;
}