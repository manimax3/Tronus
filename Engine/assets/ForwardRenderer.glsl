#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;

uniform mat4 mvp;

out struct VS_out {
    vec2 uv;
} vs_out;

void main()
{
    vs_out.uv    = uv;
    gl_Position  = mvp * vec4(position.x, position.y, position.z, 1.0);
}

#shader fragment
#version 330 core

in struct VS_out {
    vec2 uv;
} vs_out;

uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform float shininess;

out vec4 color;

void main()
{
    color = vec4(1.0);
}

