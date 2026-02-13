#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec3 aColor;

out PARAMETER_STRUCTURE {
    vec3 color;
} vs_out;

void main() {
    vs_out.color = aColor;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}