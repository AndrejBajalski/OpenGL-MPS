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
    // ambient
//    Light.ambient=vec3(0.1f);
//    Light.specular=vec3(1.0f);
//    Light.diffuse=vec3(0.8f);
//    Material.specular=color;
//    Material.diffuse=color;
//    Material.ambient=color;
//    vec3 ambient = light.ambient * Material.ambient;
//
//    // diffuse
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(light.position - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = light.diffuse * (diff * material.diffuse);
//
//    // specular
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = light.specular * (spec * material.specular);
//
//    vec3 result = ambient + diffuse + specular;
    vec2 centered = texCoord * 2.0 - 1.0;
    float a = 1.0 - smoothstep(0.9, 1.0, length(centered));
    FragColor = vec4(fColor, a);
}
