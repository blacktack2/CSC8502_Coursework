#version 400 core

layout(quads, equal_spacing, ccw) in;

uniform sampler2D heightTex;

uniform mat4 modelMatrix;
uniform mat4 projViewMatrix;

uniform float maxHeight;

void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;
	vec4 position = mix(mix(p0, p1, u), mix(p2, p3, u), v);

	vec4 worldPos = modelMatrix * position;

	float height = texture(heightTex, gl_TessCoord.xy).r;
	worldPos.y += height * maxHeight;
	gl_Position = projViewMatrix * worldPos;
}
