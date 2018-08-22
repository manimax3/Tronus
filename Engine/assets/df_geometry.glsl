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
    vec4 wp     = model * vec4(position, 1.0);
    FragPos     = wp.xyz;
    TexCoords   = uv;
    Normal      = transpos(inverse(mat3(model))) * normal;
    gl_Position = projection * view * wp;
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

layout(location = 0) out vec3 oPosition;
layout(location = 1) out vec3 oNormal;
layout(location = 3) out vec4 oDiffuse;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;

void main()
{
    oPosition    = FragPos;
    oNormal      = normalize(Normal);
    oDiffuse.rgb = texture(diffuse, TexCoords).rgb;
    oDiffuse.a   = texture(specular).r;
}
