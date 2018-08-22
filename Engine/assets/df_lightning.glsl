#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 TexCoords;

void main()
{
    TexCoords   = uv;
    gl_Position = vec4(position, 1.0);
}

#shader fragment
#version 330 core

in vec2 TexCoords;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D diffuse;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
    // retrieve data from gbuffer
    vec3  FragPos  = texture(position, TexCoords).rgb;
    vec3  Normal   = texture(normal, TexCoords).rgb;
    vec3  Diffuse  = texture(diffuse, TexCoords).rgb;
    float Specular = texture(diffuse, TexCoords).a;

    // then calculate lighting as usual
    vec3 lighting = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for (int i = 0; i < NR_LIGHTS; ++i) {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if (distance < lights[i].Radius) {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse
                = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
            // specular
            vec3  halfwayDir = normalize(lightDir + viewDir);
            float spec       = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3  specular   = lights[i].Color * spec * Specular;
            // attenuation
            float attenuation = 1.0
                / (1.0 + lights[i].Linear * distance
                   + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }
    FragColor = vec4(lighting, 1.0);
}
