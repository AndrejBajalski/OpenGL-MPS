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
vec3 blackbody(float kelvin) {
    float r, g, b;
    kelvin/=100;
    // Red
    if (kelvin <= 66.0)
        r = 255.0;
    else {
        r = kelvin - 60.0;
        r = 329.698727446 * pow(r, -0.1332047592);
        r = clamp(r, 0.0, 255.0);
    }
    // Green
    if (kelvin <= 66.0) {
        g = kelvin;
        g = 99.4708025861 * log(g) - 161.1195681661;
        g = clamp(g, 0.0, 255.0);
    } else {
        g = kelvin - 60.0;
        g = 288.1221695283 * pow(g, -0.0755148492);
        g = clamp(g, 0.0, 255.0);
    }
    // Blue
    if (kelvin >= 66.0) {
        b = 255.0;
    } else if (kelvin <= 19.0) {
        b = 0.0;
    } else {
        b = kelvin - 10.0;
        b = 138.5177312231 * log(b) - 305.0447927307;
        b = clamp(b, 0.0, 255.0);
    }
    return vec3(r, g, b) / 255.0;
}
vec3 smoke(float temperature) {
    float coeff = temperature/1000.0f;
    return vec3(1/coeff, 1/coeff, 1/coeff);
}

//in vec3 FragPos;
//in vec3 Normal;
in vec2 texCoord;
in float fTemp;

uniform vec3 viewPos;
uniform float alpha;
uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, texCoord);
    vec3 resColor;
    if(texColor.a < 0.1) discard;
    if(fTemp < 1000.0f){
        resColor = smoke(fTemp);
    }else {
        resColor = blackbody(fTemp);
    }
    FragColor = texColor * vec4(resColor, 1.0);
}
