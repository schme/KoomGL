#version 440

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 Texcoord;

out vec2 texcoord;

void main()
{
    texcoord = Texcoord;
    gl_Position = position;
}
