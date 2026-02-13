#version 330 core

layout (location = 0) in vec3	aPosition;
layout (location = 1) in vec3	aColor;

uniform mat4					matModel;
uniform mat4					matView;
uniform mat4					matProjection;

out vec3						varyingColor;

void main(void) {
	varyingColor	= aColor;
	gl_Position		= matProjection * matView * matModel * vec4(aPosition, 1.0);
}