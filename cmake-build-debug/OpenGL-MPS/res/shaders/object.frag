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
//uniform Light lightPoints[MAX_LIGHT_POINTS];
uniform Light light;
uniform float alpha;

vec3 calculatePointLight(vec3 textureMapping, vec3 norm) {
    vec3 ambient, diffuse, specular;
    for (int i=0; i<MAX_LIGHT_POINTS; i++){
        vec3 lightDir = normalize(pointPositions[i] - FragPos);
        //attenuation
        float dist = length(pointPositions[i] - FragPos);
        float attenuation = 1/(1.0 + 0.09*dist + 1.2*dist*dist);
        float coeff = 1.0/MAX_LIGHT_POINTS;
        // ambient
        ambient += light.ambient * textureMapping * attenuation * coeff;
        // diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += light.diffuse * diff * textureMapping * attenuation * coeff;
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular += light.specular * spec * material.specular * attenuation * coeff;
    }
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 textureMapping = vec3(texture(material.diffuse, TexCoord));
    // ambient
    vec3 ambient = light.ambient * textureMapping;
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * textureMapping;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    //attenuation
    float dist = length(light.position - FragPos);
    float attenuation = 1/(1.0 + 2.0*dist + 2.9*dist*dist);
//    vec3 result = (ambient + diffuse + specular)*attenuation;
    vec3 result = calculatePointLight(textureMapping, norm);
    FragColor = vec4(result, alpha);
}
