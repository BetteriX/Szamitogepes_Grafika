#version 330

layout (location = 0) in vec2	aPosition;	// attributum position

uniform	mat4					matModelView;
uniform	mat4					matProjection;

uniform	float					offsetX;
uniform	float					offsetY;

uniform float redColor;
uniform float greenColor;
uniform float blueColor;

out vec3 uniformColor;
out vec3 aColor;

out		vec2					varyingPosition;

void main(void) {
	varyingPosition = aPosition;

	//aColor = uniformColor;
	
	aColor = vec3(uniformColor.y, uniformColor.x, uniformColor.z);

	//uniformColor = vec3(redColor, greenColor, blueColor);

	//if(offsetX > 0){
	//	uniformColor = vec3(0,1,0);
	//}
	//else{
	//	uniformColor = vec3(0,0,1);
	//}

	gl_Position = matProjection * matModelView * vec4(aPosition + vec2(offsetX, offsetY), 0.0, 1.0);
}