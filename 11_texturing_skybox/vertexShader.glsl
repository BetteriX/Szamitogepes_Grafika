#version 420

layout (location = 0) in vec3	aPosition;
layout (location = 1) in vec2	aTexture;

out vec2						varyingTextureCoord;

uniform mat4					matModel;
uniform mat4					matView;
uniform mat4					matProjection;

void main(void) {
	varyingTextureCoord	= aTexture;
	gl_Position			= matProjection * matView * matModel * vec4(aPosition, 1.0);
}
