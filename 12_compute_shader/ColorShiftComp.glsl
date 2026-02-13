#version 430 core

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1)	in;
layout (rgba32f, binding = 0) uniform image2D					imgOutput;
layout (location = 0) uniform float								time;

void main() {
	const float	width		= 1024.0; // the width of the texture
	const float	speed		= 100.0;
	const ivec2	texelCoord	= ivec2(gl_GlobalInvocationID.xy);
	vec4		value		= vec4(0.0, 0.0, 0.0, 1.0);
	// value will be a color mix of red and green
	value.xy = mod(texelCoord.xy + time * speed, width) / (gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
	imageStore(imgOutput, texelCoord, value);
}