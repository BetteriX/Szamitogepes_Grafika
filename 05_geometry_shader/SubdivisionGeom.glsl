#version 330 core
// Produce PI value on each architecture.
const float M_PI = asin(1.0) * 2.0;
// Alright, let's cook up some shader magic.
// Here is a simple example of a geometry shader that takes a triangle as input and 
// subdivides it into smaller triangles to approximate a circle.
layout (triangles)                          in;
layout (triangle_strip, max_vertices = 146) out;    // 146 -> 1024 / 7 component (xyzw, rgb)
//layout (triangle_strip, max_vertices = gl_MaxGeometryOutputVertices / 7) out;    // 146 -> 1024 / 7 component (xyzw, rgb)
in PARAMETER_STRUCTURE {
    vec3 color;
} gs_in[];

uniform mat4    matModelView;
uniform mat4    matProjection;
uniform int     geometryDetail;

out vec3        varyingColor;
// https://en.wikipedia.org/wiki/Circumcircle#Circumcenter_coordinates
vec2 circumcenter(vec2 A, vec2 B, vec2 C) {
    float D = 2.0 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
    float Ux = ((A.x * A.x + A.y * A.y) * (B.y - C.y) +
                (B.x * B.x + B.y * B.y) * (C.y - A.y) +
                (C.x * C.x + C.y * C.y) * (A.y - B.y)) / D;
    float Uy = ((A.x * A.x + A.y * A.y) * (C.x - B.x) +
                (B.x * B.x + B.y * B.y) * (A.x - C.x) +
                (C.x * C.x + C.y * C.y) * (B.x - A.x)) / D;

    return vec2(Ux, Uy);
}

vec2 v0 = gl_in[0].gl_Position.xy;
vec2 v1 = gl_in[1].gl_Position.xy;
vec2 v2 = gl_in[2].gl_Position.xy;
vec2 center = circumcenter(v0, v1, v2);
vec4 center_vec4 = vec4(center, 0.0, 1.0);
const vec3 centerColor = vec3(1.0);
float r = length(center - v0);

void subdivision(float angle0, float angle1, vec3 color0, vec3 color1, int depth) {
    int     limit       = int(pow(2, depth));
    float   alpha       = angle0;
    float   alpha_step  = angle1 - angle0; 
    // alpha_step shall be in [-M_PI, M_PI] range
    while (alpha_step < -M_PI)
        alpha_step += 2 * M_PI;
    while (alpha_step > M_PI)
        alpha_step -= 2 * M_PI;
    // alpha_step will be divided into 2^depth pieces
    alpha_step /= limit;
    // output can be just triangle STRIP, NOT triangle...
    for (int i = 0; i < limit; i++, alpha += alpha_step) {
        varyingColor = centerColor;
        gl_Position = matProjection * matModelView * center_vec4;
        EmitVertex();

        varyingColor = (color0 * (limit - i) + color1 * i ) / limit;
        gl_Position = matProjection * matModelView * vec4(cos(alpha) * r, sin(alpha) * r, 0.0, 1.0);
        EmitVertex();

        varyingColor = (color0 * (limit - i - 1) + color1 * (i + 1)) / limit;
        gl_Position = matProjection * matModelView * vec4(cos(alpha + alpha_step) * r, sin(alpha + alpha_step) * r, 0.0, 1.0);
        EmitVertex();
        // ...so we close the strips after 3 elements to get 1 triangle
        EndPrimitive();
    }
    return;
}
// This shader uses the dot product to find the angle between the normalized direction vector and a reference vector along the x-axis.
// It then adjusts the angle based on the sign of the cross product to ensure it falls within the range from 0 to 2*PI.
//          Center point Other point
float angle(vec2 center, vec2 point) {
    vec2    direction       = normalize(point - center);
    vec2    reference       = vec2(1.0, 0.0); // Reference vector along x-axis.
    float   dotProduct      = dot(direction, reference);
    float   crossProduct    = direction.y * reference.x - direction.x * reference.y;
    float   angleRad        = acos(dotProduct);
    // let's normalize
    if (crossProduct < 0.0)
        angleRad = 2.0 * M_PI - angleRad;

    return angleRad;
}
// You can adjust geometryDetail to control the number of subdivisions, which in turn affects how closely the 
// resulting shape approximates a circle. This shader takes the vertices of an input triangle, performs 
// barycentric interpolation to generate new vertices, and emits those vertices in a triangle strip format.
void main() {
    float angle0 = angle(center, v0);
    float angle1 = angle(center, v1);
    float angle2 = angle(center, v2);
    subdivision(angle0, angle1, gs_in[0].color, gs_in[1].color, geometryDetail);
    subdivision(angle1, angle2, gs_in[1].color, gs_in[2].color, geometryDetail);
    subdivision(angle2, angle0, gs_in[2].color, gs_in[0].color, geometryDetail);
}