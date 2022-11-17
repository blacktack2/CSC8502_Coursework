#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D normTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

uniform vec3 ambienceColour;

uniform int mode;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	if (mode == 1) {
		vec4 normal = texture(normTex, IN.texCoord);
		fragColour.rgb = normal.rgb;
	} else {
		vec4 diffuse = texture(diffuseTex, IN.texCoord);
		vec4 normal = texture(normTex, IN.texCoord);
		if (normal.w == 0.0) {
			fragColour = vec4(diffuse.rgb, 1.0);
			return;
		}
		vec3 light = texture(diffuseLight, IN.texCoord).rgb;
		vec3 spec = texture(specularLight, IN.texCoord).rgb;

		fragColour.rgb = diffuse.rgb * light;
		fragColour.rgb += spec;
		fragColour.rgb += diffuse.rgb * ambienceColour;
	}
	fragColour.a = 1.0;
}
