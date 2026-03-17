#version 330

in vec3 uniformColor;
in	vec2	varyingPosition;

in vec3 aColor;

out vec4	outColor;

void main(void) {
	outColor = vec4(aColor, 1.0);
	//outColor = vec4(uniformColor, 1.0);
	//outColor = vec4(0.5 + varyingPosition.x, 0.5 + varyingPosition.y, 0.5, 1.0);
}