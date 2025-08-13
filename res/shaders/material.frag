#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//in vec3 FragPos;
//in vec3 Normal;
in vec3 fColor;
in vec2 texCoord;

uniform vec3 viewPos;
uniform float alpha;

void main()
{
    vec2 centered = texCoord * 2.0 - 1.0;
    float a = 1.0 - smoothstep(0.9, 1.0, length(centered));
    FragColor = vec4(fColor, a);
}
