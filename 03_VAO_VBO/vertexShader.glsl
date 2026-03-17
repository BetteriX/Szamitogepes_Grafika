#version 330

layout (location = 0) in vec2	aPosition;	// attributum position
layout (location = 1) in vec3	AColor;


out		vec2					varyingPosition;
out		vec3					vColor;

void main(void) {
	varyingPosition = aPosition;
	vColor = AColor;
	gl_Position = vec4(aPosition, 0.0, 1.0);
}