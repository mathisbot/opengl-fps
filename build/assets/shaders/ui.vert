#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aTexCoords;
    mat4 viewFixed = mat4(mat3(view));
    vec4 pos = projection * viewFixed * vec4(aPos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, pos.w);
}