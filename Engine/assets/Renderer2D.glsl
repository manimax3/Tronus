#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 iColor;
layout(location = 2) in int _tid;
layout(location = 3) in vec2 uv;

uniform mat4 vp = mat4(1.0);

out struct VS_out {
    vec4 color;
    vec2 uv;
} vs_out;

flat out int tid;

void main()
{
    vs_out.color = iColor;
    tid   = _tid;
    vs_out.uv    = uv;
    gl_Position  = vp * vec4(position.x, position.y, 0.0, 1.0);
}

#shader fragment
#version 330 core

in struct VS_out {
    vec4 color;
    vec2 uv;
} vs_out;

flat in int tid;

uniform sampler2D tex0;

out vec4 color;

void main()
{
    if (tid < 0) {
        color = vs_out.color;
    } else {
        color = texture(tex0, vs_out.uv) * vs_out.color;
    }
}

