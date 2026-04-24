#version 330

in vec3         varyingFragmentPosition;
in vec3         varyingColor;
in vec3         varyingNormal;

uniform vec3    lightPosition;
uniform vec3    cameraPosition;

uniform vec3    emissive;
uniform vec3    matColor[3];

uniform vec3    lightPositions[4];
uniform int     lightCount;

out vec4        outColor;
// AMBIENT
vec3            ambientMaterial = varyingColor;
//vec3            ambientMaterial = matColor[0];
const vec3      ambientColor = vec3(1.0, 1.0, 1.0);                             // white
const float     ambientStrength = 0.25;
vec3      ambient = ambientStrength * ambientColor * ambientMaterial;

void main(void) {
    for (int i = 0; i < lightCount; i++) {
    
        // DIFFUSE
        vec3            diffuseMaterial = varyingColor;
        //vec3            diffuseMaterial = matColor[1];
        const vec3      diffuseColor = vec3(212.0 / 255.0, 175.0 / 255.0, 55.0 / 255.0);// gold
        const float     diffuseStrength = 1.0;
        vec3            normalizedNormal = normalize(varyingNormal);
        vec3            lightDirection = normalize(lightPositions[i] - varyingFragmentPosition);
        float           diffuseCoEfficient = max(dot(normalizedNormal, lightDirection), 0.0);
        vec3            diffuse = diffuseStrength * diffuseCoEfficient * diffuseColor;
        ambient += diffuse * diffuseMaterial;

        // SPECULAR
        vec3            specularMaterial = varyingColor;
        //vec3            specularMaterial = matColor[2];
        const vec3      specularColor = vec3(212.0 / 255.0, 175.0 / 255.0, 55.0 / 255.0);// gold
        const float     specularStrength = 1.0;
        const int       specularShininess = 32; // higher value gives more pronounced effect
        vec3            viewDirection = normalize(cameraPosition - varyingFragmentPosition);
        vec3            reflectDirection = reflect(-lightDirection, normalizedNormal);
        float           specularCoEfficient = pow(max(dot(viewDirection, reflectDirection), 0.0), specularShininess);
        vec3            specular = specularStrength * specularCoEfficient * specularColor;
        ambient += specular * specularMaterial;
    }

    outColor = vec4(emissive * 0.25 + ambient, 1.0);
}