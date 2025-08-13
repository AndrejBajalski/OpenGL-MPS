#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 fColor;
out vec2 texCoord;

void main()
{
    fColor = aColor;
    texCoord = aUV;
//    gl_Position = projection * view * vec4(aPos+aOffset, 1.0);
    gl_Position = vec4(aPos+aOffset, 1.0);

}
