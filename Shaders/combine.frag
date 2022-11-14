#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D ambienceTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec3 diffuse = texture(diffuseTex, IN.texCoord).rgb;
	vec4 ambience = texture(ambienceTex, IN.texCoord);
	if (ambience.w == 0.0) {
		fragColour = vec4(diffuse, 1.0);
		return;
	}
	vec3 light = texture(diffuseLight, IN.texCoord).rgb;
	vec3 spec = texture(specularLight, IN.texCoord).rgb;

	fragColour.xyz = diffuse * light;
	fragColour.xyz += spec;
	fragColour.xyz += diffuse * ambience.xyz;
	fragColour.w = 1.0;
}
