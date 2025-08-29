#version 330 core
layout (location = 0) in vec3 oPos;
layout (location = 1) in vec3 oNormal;
layout (location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 camRight = normalize(vec3(view[0][0], view[1][0], view[2][0]));
    vec3 camUp    = normalize(vec3(view[0][1], view[1][1], view[2][1]));
    vec3 world = camRight*oPos.x + camUp*oPos.y;
    FragPos = world;
    Normal = normalize(-vec3(view[0][2], view[1][2], view[2][2])); // camera forward
//    FragPos = vec3(model * vec4(oPos, 1.0));
//    Normal = mat3(transpose(inverse(model))) * oNormal;
    TexCoord = texCoord;
    gl_Position = projection * view * model * vec4(FragPos, 1.0);
}

