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
out mat3 TBN;

void main()
{
    vec4 wp     = model * vec4(position, 1.0);
    FragPos     = wp.xyz;
    TexCoords   = uv;
    Normal      = transpose(inverse(mat3(model))) * normal;
    gl_Position = projection * view * wp;

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    TBN    = mat3(T, B, N);
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

layout(location = 0) out vec3 oPosition;
layout(location = 1) out vec3 oNormal;
layout(location = 2) out vec4 oDiffuse;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;

uniform int usenormalmap = 0;

void main()
{
    oPosition = FragPos;
    oNormal   = normalize(Normal);
    if (usenormalmap > 0) {
        oNormal = normalize(TBN * texture(normal, TexCoords).rgb);
    } else {
        oNormal = normalize(Normal);
    }
    oDiffuse.rgb = texture(diffuse, TexCoords).rgb;
    oDiffuse.a   = texture(specular, TexCoords).r;
}
