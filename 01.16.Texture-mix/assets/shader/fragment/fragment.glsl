#version 460 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D grassSampler;
uniform sampler2D landSampler;
uniform sampler2D noiseSampler;

void main()
{
    vec4 grassColor = texture(grassSampler, uv);
    vec4 landColor = texture(landSampler, uv);
    vec4 noiseColor = texture(noiseSampler, uv);
    FragColor = mix(grassColor, landColor, noiseColor.r);
}
