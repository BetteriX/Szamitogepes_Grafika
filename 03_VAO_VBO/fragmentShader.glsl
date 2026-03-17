#version 330

in	vec2	varyingPosition;
in	vec3	vColor;
out vec4	outColor;

void main(void) {
	//outColor = vec4(0.5 + varyingPosition.x, 0.5 + varyingPosition.y, 0.5, 1.0);
	//outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//outColor = vec4(vColor,1.0f);
	outColor = vec4(vColor.y, vColor.z, vColor.x, 1.0f);
}