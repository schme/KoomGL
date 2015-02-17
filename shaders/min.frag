#version 440
out vec4 color;
void main()
{
    vec2 uv = gl_FragCoord.xy; // not really uv
    color = vec4( sin(uv.x/1000), sin(uv.y/1000), sin(uv/10));
}
