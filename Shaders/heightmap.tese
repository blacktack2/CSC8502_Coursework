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
	vec3 normal;
	vec4 tangent;
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

	vec3 n0 = IN[0].normal;
	vec3 n1 = IN[1].normal;
	vec3 n2 = IN[2].normal;
	vec3 n3 = IN[3].normal;
	vec3 normal = mix(mix(n0, n1, u), mix(n2, n3, u), v);

	vec4 ta0 = IN[0].tangent;
	vec4 ta1 = IN[1].tangent;
	vec4 ta2 = IN[2].tangent;
	vec4 ta3 = IN[3].tangent;
	vec4 tangent = mix(mix(ta0, ta1, u), mix(ta2, ta3, u), v);

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wTangent, wNormal) * tangent.w;

	vec4 worldPos = modelMatrix * position;

	float height = texture(heightTex, gl_TessCoord.xy).r;
	worldPos.y += height * maxHeight;

	OUT.worldPos = worldPos.xyz;
	gl_Position = projMatrix * viewMatrix * worldPos;
}
