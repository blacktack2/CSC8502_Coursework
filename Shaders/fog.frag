#version 330 core
uniform sampler2D sceneTex;
uniform sampler2D depthTex;

uniform float fogMin;
uniform float fogMax;
uniform vec3 fogColour;

uniform vec3 cameraPos;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	float depth = texture(depthTex, IN.texCoord).r;
	vec3 scene = texture(sceneTex, IN.texCoord).rgb;
	if (depth == 1) {
		fragColour = vec4(scene, 1.0);
	} else {
		float depthScale = fogMax - fogMin;
		if (depth > fogMax)
			fragColour = vec4(fogColour, 1.0);
		else if (depth < fogMin)
			fragColour = vec4(scene, 1.0);
		else
			fragColour = vec4(mix(scene, fogColour, (depth - fogMin) / depthScale), 1.0);
	}
}