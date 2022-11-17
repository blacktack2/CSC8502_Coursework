#version 400 core

layout(quads, equal_spacing, ccw) in;

uniform sampler2D heightTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float maxHeight;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;
	vec4 position = mix(mix(p0, p1, u), mix(p2, p3, u), v);

	vec4 c0 = IN[0].colour;
	vec4 c1 = IN[1].colour;
	vec4 c2 = IN[2].colour;
	vec4 c3 = IN[3].colour;
	OUT.colour = mix(mix(c0, c1, u), mix(c2, c3, u), v);

	vec2 tx0 = IN[0].texCoord;
	vec2 tx1 = IN[1].texCoord;
	vec2 tx2 = IN[2].texCoord;
	vec2 tx3 = IN[3].texCoord;
	OUT.texCoord = mix(mix(tx0, tx1, u), mix(tx2, tx3, u), v);

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	
	float height = textureOffset(heightTex, gl_TessCoord.xy, ivec2(-10, -10)).r * maxHeight;

	float h10 = textureOffset(heightTex, gl_TessCoord.xy, ivec2(1, 0)).r * maxHeight;
	float h01 = textureOffset(heightTex, gl_TessCoord.xy, ivec2(0, 1)).r * maxHeight;

	OUT.binormal  = normalize(vec3(1.0, height - h10, 0.0));
	OUT.tangent = normalize(vec3(0.0, height - h01, 1.0));
	OUT.normal = normalize(cross(OUT.tangent, OUT.binormal));

	vec4 worldPos = modelMatrix * (position + vec4(0.0, height, 0.0, 0.0));

	OUT.worldPos = worldPos.xyz;
	gl_Position = projMatrix * viewMatrix * worldPos;
}
