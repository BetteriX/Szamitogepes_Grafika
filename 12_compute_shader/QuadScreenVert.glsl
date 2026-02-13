#version 430 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

uniform mat4    matModelView;
uniform mat4    matProjection;

out vec2        varyingTextureCoords;

void main() {
    varyingTextureCoords = aTexCoords;
    gl_Position = matProjection * matModelView * vec4(aPosition, 0.0, 1.0);
}