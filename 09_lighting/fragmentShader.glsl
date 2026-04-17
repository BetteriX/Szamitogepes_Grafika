#version 330

in vec3         varyingFragmentPosition;
in vec3         varyingNormal;

//in vec3 lColor[];
uniform vec3 lightColor[];

uniform vec3    lightPosition;
uniform vec3    cameraPosition;
uniform vec3 positions;
uniform int numPos;

out vec4        outColor;
// AMBIENT
//const vec3      ambientColor = vec3(0.0, 0.0, 1.0);                             // white
vec3      ambientColor = lightColor[0];                             // white
const float     ambientStrength = 0.25;
vec3      ambient = ambientStrength * ambientColor;


void main(void) {

    for( int i = 0; i < numPos; i++){
        // DIFFUSE
        //const vec3      diffuseColor = vec3(212.0 / 255.0, 175.0 / 255.0, 55.0 / 255.0);// gold
        vec3      diffuseColor = lightColor[1];
        const float     diffuseStrength = 1.0;
        vec3            normalizedNormal = normalize(varyingNormal);
        vec3            lightDirection = normalize(positions[i] - varyingFragmentPosition);
        float           diffuseCoEfficient = max(dot(normalizedNormal, lightDirection), 0.0);
                    ambient += diffuseStrength * diffuseCoEfficient * diffuseColor;

        // SPECULAR
        //const vec3      specularColor = vec3(212.0 / 255.0, 175.0 / 255.0, 55.0 / 255.0);// gold
        //const vec3      specularColor = vec3(1.0f, 0.0f, 0.0f);// gold
        vec3      specularColor = lightColor[2];
        const float     specularStrength = 1.0;
        const int       specularShininess = 32; // higher value gives more pronounced effect
        vec3            viewDirection = normalize(cameraPosition - varyingFragmentPosition);
        vec3            reflectDirection = reflect(-lightDirection, normalizedNormal);
        float           specularCoEfficient = pow(max(dot(viewDirection, reflectDirection), 0.0), specularShininess);
                  ambient += specularStrength * specularCoEfficient * specularColor; 
    }
    

    outColor = vec4(ambient, 1.0);
}