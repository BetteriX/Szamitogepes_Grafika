#version 330 core
// (points): This indicates that the input primitive type is points.
layout (points)                             in;
layout (triangle_strip, max_vertices = 146) out;    // 146 -> 1024 / 7 component (xyzw, rgb)
// outgoing structure for generated
in PARAMETER_STRUCTURE {
    vec3 color;
} gs_in[];

uniform mat4    matModelView;
uniform mat4    matProjection;
uniform int     geometryDetail;

out vec3        varyingColor;
// This formula produces PI value on each architecture.
const float     M_PI = asin(1.0) * 2.0;
const float     size = 0.1;

void build_house(vec4 position) {    
    varyingColor = gs_in[0].color;                              // gs_in[0] since there's only one input vertex
    gl_Position = position + vec4(-size, -size, 0.0, 0.0);      // 1:bottom-left
    gl_Position = matProjection * matModelView * gl_Position;   // matProjection * matModelView * gl_Position shall be here, NOT in vertex shader!
    EmitVertex();   
    gl_Position = position + vec4( size, -size, 0.0, 0.0);      // 2:bottom-right
    gl_Position = matProjection * matModelView * gl_Position;
    EmitVertex();
    gl_Position = position + vec4(-size,  size, 0.0, 0.0);      // 3:top-left
    gl_Position = matProjection * matModelView * gl_Position;
    EmitVertex();
    gl_Position = position + vec4( size,  size, 0.0, 0.0);      // 4:top-right
    gl_Position = matProjection * matModelView * gl_Position;
    EmitVertex();
    gl_Position = position + vec4( 0.0,  2.0 * size, 0.0, 0.0); // 5:top
    gl_Position = matProjection * matModelView * gl_Position;
    varyingColor = vec3(1.0, 1.0, 1.0);                         // house roof top is white
    EmitVertex();
    EndPrimitive();
}

void main() {
    float r             = 0.0;
    float alpha         = 0.0;
    float r_step        = M_PI / 15.0; 
    float alpha_step    = M_PI / 4.0;
    // making a spiral of houses
    for (int i = 0; i <= geometryDetail; i++, alpha += alpha_step, alpha_step *= 0.95, r += r_step, r_step *= 0.88)
        build_house(gl_in[0].gl_Position + vec4(cos(alpha) * r, sin(alpha) * r, 0.0, 1.0));
}