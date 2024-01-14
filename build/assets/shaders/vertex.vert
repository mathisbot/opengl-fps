#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

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

out vec2 TexCoords;
out vec3 FragPos;
out vec3 TangentLightPos[NR_POINT_LIGHTS];
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;


void main()
{
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // Expensive, should be done only once per model
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));

    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        TangentLightPos[i] = TBN * pointLights[i].position;
    }
    TangentViewPos = TBN * viewPos;
    TangentFragPos = TBN * FragPos;
}