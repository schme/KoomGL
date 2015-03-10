#version 440

in vec2 texcoord;

uniform float globalTime;
uniform sampler2D sampler;

void main()
{
    gl_FragColor = texture( sampler, texcoord);
}
