#version 330 core

uniform vec3 oColor;

void main() {
    gl_FragColor = vec4(oColor, 1.0);
}
