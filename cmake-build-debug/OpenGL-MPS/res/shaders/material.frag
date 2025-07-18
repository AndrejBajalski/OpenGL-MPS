#version 330 core
out vec4 FragColor;

// texture samplers
//uniform sampler2D texture1;
//uniform sampler2D texture2;
uniform int particleType;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    vec3 color;
    if(particleType==0)
        color = vec3(0.0f, 0.0f, 1.0f);
    else if(particleType==2)
        color = vec3(1.0f, 1.0f, 0.0f);
    else
        color = vec3(1.0f, 1.0f, 1.0f);

    FragColor = vec4(color, 1.0f);
}