#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 mvp;

void main() { gl_Position = mvp * vec4(position, 1.0); }

#shader fragment
#version 330 core

#define ambient_intensity 1.0
#define diffuse_intensity 1.0 

out vec4 FragColor;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D diffuse;

struct Light {
    vec3 Position;
    vec3 Color;

    float Constant;
    float Linear;
    float Quadratic;
};

uniform Light light;
uniform vec3 viewpos;
uniform vec2 screensize;

vec2 CalcTexCoord() { return gl_FragCoord.xy / screensize; }

vec4 calc_point_light(vec3 worldpos, vec3 normal, float spec)
{
    vec3  LightDirection = viewpos - light.Position;
    float distance       = length(LightDirection);
    LightDirection       = normalize(LightDirection);

    vec4  ambient_color = vec4(light.Color * ambient_intensity, 1.0);
    float DiffuseFactor = dot(normal, -LightDirection);
    vec4  DiffuseColor  = vec4(0, 0, 0, 0);
    vec4  SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor
            = vec4(light.Color * diffuse_intensity * DiffuseFactor, 1.0);
        vec3  VertexToEye    = normalize(viewpos - worldpos);
        vec3  LightReflect   = normalize(reflect(VertexToEye, normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        if (SpecularFactor > 0) {
            SpecularFactor = pow(SpecularFactor, spec);
            SpecularColor  = vec4(light.Color * spec * SpecularFactor, 1.0);
        }
    }

    vec4 color = vec4(ambient_color + DiffuseColor + SpecularColor);

    float atten = light.Constant + light.Linear * distance
        + light.Quadratic * distance * distance;
    atten = max(1.0, atten);

    return color / atten;
}

void main()
{
    // retrieve data from gbuffer
    vec2  TexCoords = CalcTexCoord();
    vec3  FragPos   = texture(position, TexCoords).rgb;
    vec3  Normal    = texture(normal, TexCoords).rgb;
    vec3  Diffuse   = texture(diffuse, TexCoords).rgb;
    float Specular  = texture(diffuse, TexCoords).a;

    FragColor
        = vec4(Diffuse * calc_point_light(FragPos, Normal, Specular).xyz, 1.0);
}
