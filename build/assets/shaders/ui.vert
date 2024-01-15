#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;

const float theta = -1.2;
const float factor = 1.0;
const float phi = -0.2;

void main()
{
    // TODO: Manually compute the product of the matrices
    mat4 model = mat4(
        vec4(factor*cos(theta), 0.0, factor*sin(theta), 0.0),
        vec4(0.0, factor, 0.0, 1.5),
        vec4(-factor*sin(theta), 0.0, factor*cos(theta), 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    mat4 rot = mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, cos(phi), sin(phi), 0.0),
        vec4(0.0, -sin(phi), cos(phi), 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    mat4 tot = mat4(
        vec4(0.349, 0.0, -0.936, 0.0),
        vec4(0.0, 0.982, 0.188, 0.0),
        vec4(0.936, -0.188, 0.349, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    TexCoords = aTexCoords;
    vec4 pos = projection * rot * model * vec4(aPos, 1.0);
    gl_Position = vec4(pos.x+0.11, pos.y-0.35, pos.z, pos.w);
}
