#version 460 core
out vec4 FragColor;

in vec3 ourColor;

uniform float timeVal;

void main()
{
    float scale1 = (sin(timeVal) + 1.0f) / 2.0f; // Scale oscillates between 0 and 1
    float scale2 = (cos(timeVal) + 1.0f) / 2.0f; // Scale oscillates between 0 and 1
    FragColor = vec4(ourColor.r * scale1, ourColor.g * scale2, ourColor.b, 1.0f);
}
