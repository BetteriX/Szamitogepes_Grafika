#version 330

layout (location = 0) in vec3	aPosition;

uniform mat4					matModel;
uniform mat4					matView;
uniform mat4					matProjection;

void main(void) {
	gl_Position =  matProjection * matView * matModel * vec4(aPosition, 1.0);
}