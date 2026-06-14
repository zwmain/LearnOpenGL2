#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;

out vec2 uv;

uniform mat4 trans;

void main()
{
    gl_Position = trans * vec4(aPos, 1.0);
    uv = aUv;
}
