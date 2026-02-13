#version 330

layout (location = 0) in vec2	aPosition;	// attributum position

uniform	mat4					matModelView;
uniform	mat4					matProjection;

uniform	float					offsetX;
uniform	float					offsetY;

out		vec2					varyingPosition;

void main(void) {
	varyingPosition = aPosition;
	gl_Position = matProjection * matModelView * vec4(aPosition + vec2(offsetX, offsetY), 0.0, 1.0);
}