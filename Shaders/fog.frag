#version 330 core
uniform sampler2D sceneTex;
uniform sampler2D depthTex;

uniform mat4 inverseProjView;

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
		//vec3 ndcPos = vec3(IN.texCoord, depth) * 2.0 - 1.0;
		//vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
		//vec3 worldPos = invClipPos.xyz / invClipPos.w;

		float depthScale = fogMax - fogMin;
		float t = clamp((depth - fogMin) / depthScale, 0.0, 1.0);
		fragColour = vec4(mix(scene, fogColour, t), 1.0);
	}
}