#version 420

layout (binding=0) uniform sampler2D    textureSampler;
layout (binding=1) uniform sampler2D    textureSampler2;

in vec2                                 varyingTextureCoord;

out vec4                                outColor;


void main(void) {
    vec4 tegla = texture(textureSampler, varyingTextureCoord); 
    vec4 graffiti = texture(textureSampler2, varyingTextureCoord); 
    outColor = mix(tegla, graffiti, 0.5);
}