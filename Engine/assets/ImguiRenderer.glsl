#shader vertex
#version 330 core

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;

uniform mat4 proj;

out vec2 Frag_UV;
out vec4 Frag_Color;

void main()
{
    Frag_UV     = uv;
    Frag_Color  = color;
    gl_Position = proj * vec4(Position.xy, 0, 1);
}

#shader fragment
#version 330 core

in vec2 Frag_UV;
in vec4 Frag_Color;

uniform sampler2D tex;

out vec4 Out_Color;

void main() { Out_Color = Frag_Color * texture(tex, Frag_UV.st); }
