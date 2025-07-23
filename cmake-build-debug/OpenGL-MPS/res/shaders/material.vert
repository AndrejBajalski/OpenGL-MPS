#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in mat4 instanceMatrix;

out vec3 fColor;
//out vec3 FragPos;
//out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    //    FragPos = vec3(model * vec4(aPos, 1.0));
    //    Normal = mat3(transpose(inverse(model))) * aNormal;
    fColor = aColor;
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}
