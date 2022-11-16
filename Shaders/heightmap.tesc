#version 400 core

layout(vertices = 4) out;

uniform float tessLevelInner;
uniform float tessLevelOuter;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec4 tangent;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec4 tangent;
} OUT[];

void main() {
	gl_TessLevelOuter[0] = tessLevelOuter;
	gl_TessLevelOuter[1] = tessLevelOuter;
	gl_TessLevelOuter[2] = tessLevelOuter;
	gl_TessLevelOuter[3] = tessLevelOuter;

	gl_TessLevelInner[0] = tessLevelInner;
	gl_TessLevelInner[1] = tessLevelInner;

	OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;
	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
	OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
	OUT[gl_InvocationID].tangent = IN[gl_InvocationID].tangent;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
