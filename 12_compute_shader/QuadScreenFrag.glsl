#version 430 core

in vec2             varyingTextureCoords;
uniform sampler2D   textureSampler2D;
out vec4            outColor;

void main() {             
    const vec2      TEXTURE_SIZE = vec2(1024.0, 1024.0);
// texture() goes through the filtering system
//    outColor = texture(textureSampler2D, varyingTextureCoords);
// texelFetch() direct access of texel data WITHOUT going through the filtering system
    outColor = texelFetch(textureSampler2D, ivec2(varyingTextureCoords * TEXTURE_SIZE), 0); // 0 is the highest mipmap level
}
