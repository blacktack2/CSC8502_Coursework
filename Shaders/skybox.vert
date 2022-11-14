#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

out Vertex {
	vec3 viewDir;
} OUT;

void main() {
	//vec3 pos = position;
	//mat4 invProj = inverse(projMatrix);

	//pos.xy *= vec2(invProj[0][0], invProj[1][1]);
	//pos.z = -1.0;
	//OUT.viewDir = transpose(mat3(viewMatrix)) * normalize(pos);
	gl_Position = vec4(position, 1.0);
	OUT.viewDir = transpose(mat3(viewMatrix)) * (inverse(projMatrix) * gl_Position).xyz;
}
