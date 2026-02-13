#version 420

layout (binding=0) uniform sampler2D    textureSampler;

in vec2                                 varyingTextureCoord;

out vec4                                outColor;


void main(void) {
    outColor = texture(textureSampler, varyingTextureCoord);
}