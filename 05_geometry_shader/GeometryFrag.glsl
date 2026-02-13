#version 330 core

out vec4    outColor;
in vec3     varyingColor;

void main() {             
    outColor = vec4(varyingColor, 1.0);   
}