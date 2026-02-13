#version 330

in vec3         varyingFragmentPosition;
in vec3         varyingNormal;

uniform vec3    lightPosition;
uniform vec3    cameraPosition;

out vec4        outColor;
// AMBIENT
const vec3      ambientColor = vec3(1.0, 1.0, 1.0);                             // white
const float     ambientStrength = 0.25;
const vec3      ambient = ambientStrength * ambientColor;
// DIFFUSE
const vec3      diffuseColor = vec3(212.0 / 255.0, 175.0 / 255.0, 55.0 / 255.0);// gold
const float     diffuseStrength = 1.0;
vec3            normalizedNormal = normalize(varyingNormal);
vec3            lightDirection = normalize(lightPosition - varyingFragmentPosition);
float           diffuseCoEfficient = max(dot(normalizedNormal, lightDirection), 0.0);
vec3            diffuse = diffuseStrength * diffuseCoEfficient * diffuseColor;
// SPECULAR
const vec3      specularColor = vec3(212.0 / 255.0, 175.0 / 255.0, 55.0 / 255.0);// gold
const float     specularStrength = 1.0;
const int       specularShininess = 32; // higher value gives more pronounced effect
vec3            viewDirection = normalize(cameraPosition - varyingFragmentPosition);
vec3            reflectDirection = reflect(-lightDirection, normalizedNormal);
float           specularCoEfficient = pow(max(dot(viewDirection, reflectDirection), 0.0), specularShininess);
vec3            specular = specularStrength * specularCoEfficient * specularColor; 

void main(void) {
    outColor = vec4(ambient + diffuse + specular, 1.0);
}