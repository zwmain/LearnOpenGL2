#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;

out vec2 uv;

uniform float time;

void main()
{
    float sawtooth = fract(time * 1.0) * 2.0 - 1.0;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    uv = vec2(aUv.x + sawtooth, aUv.y);
}
