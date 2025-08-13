#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in float aTemp;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out float fTemp;

void main()
{
    texCoord = aUV;
    fTemp = aTemp;
//    gl_Position = projection * view * vec4(aPos+aOffset, 1.0);
    gl_Position = vec4(aPos+aOffset, 1.0);

}
