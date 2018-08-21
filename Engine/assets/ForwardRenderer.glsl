#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos   = vec3(model * vec4(position, 1.0));
    Normal    = mat3(transpose(inverse(model))) * normal;
    TexCoords = uv;

    gl_Position = projection * view * model * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform float     shininess;

out vec4 color;

void main() { color = texture(ambient, TexCoords); }

