#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in float aTemp;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float billboardSize;

out vec2 texCoord;
out float fTemp;

void main()
{
    vec3 camRight = normalize(vec3(view[0][0], view[1][0], view[2][0]));
    vec3 camUp    = normalize(vec3(view[0][1], view[1][1], view[2][1]));
    vec3 vertexPosWorldSpace = aOffset + camRight*aPos.x + camUp*aPos.y;
    texCoord = aUV;
    fTemp = aTemp;
    gl_Position = projection * view * vec4(vertexPosWorldSpace, 1.0);
}
