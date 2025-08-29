#version 330 core
#define MAX_LIGHT_POINTS 10

layout (std140) uniform PointLights {
    vec3 pointPositions[MAX_LIGHT_POINTS];
};
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 specular;
    float shininess;
    sampler2D diffuse;
    vec3 color;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float alpha;
uniform mat4 fireModel;

vec3 calculatePointLight(vec3 textureMapping, vec3 norm) {
    vec3 ambient, diffuse, specular;
    for (int i=0; i<MAX_LIGHT_POINTS; i++){
        vec3 pPWorld = (fireModel * vec4(pointPositions[i], 1.0f)).xyz;
        vec3 lightDir = normalize(pPWorld - FragPos);
        //attenuation
        float dist = length(pPWorld - FragPos);
        float attenuation = 1/(1.0 + 0.1*dist + 1.5*dist*dist);
        float coeff = 1/MAX_LIGHT_POINTS;
        // ambient
        ambient += light.ambient * textureMapping * material.color * attenuation;
        // diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += light.diffuse * diff * textureMapping * material.color * attenuation * coeff;
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular += light.specular * spec * material.specular * attenuation * coeff;
    }
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 textureMapping = vec3(texture(material.diffuse, TexCoord));
    vec3 norm = normalize(Normal);
    vec3 result = calculatePointLight(textureMapping, norm);
    FragColor = vec4(result, 1.0f);
}
