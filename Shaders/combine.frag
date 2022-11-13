#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

uniform vec3 ambientColour;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 light = texture(diffuseLight, IN.texCoord).xyz;
	vec3 spec = texture(specularLight, IN.texCoord).xyz;

	fragColour.xyz = diffuse * light;
	fragColour.xyz += spec;
	fragColour.xyz += diffuse * ambientColour;
	fragColour.w = 1.0;
}
