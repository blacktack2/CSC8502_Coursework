#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform mat4 inverseProjView;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec4 lightPos;
uniform vec4 lightColour;

out vec4 diffuseOutput;
out vec4 specularOutput;

void main() {
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texture(depthTex, texCoord).r;
	vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz / invClipPos.w;

	vec3 incident;
	float atten;
	if (lightPos.w == 0) {
		incident = normalize(lightPos.xyz);
		atten = 1.0;
		if (length(worldPos) > lightRadius / 2)
			discard;
	} else {
		vec3 lPos = lightPos.xyz / lightPos.w;
		incident = normalize(lPos - worldPos);
		float dist = length(lPos - worldPos);
		atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);
		if (atten == 0)
			discard;
	}

	vec3 normal = normalize(texture(normTex, texCoord).xyz * 2.0 - 1.0);
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float rFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
	float specFactor = pow(rFactor, 60.0);
	vec3 attenuated = lightColour.xyz * atten;
	diffuseOutput = vec4(attenuated * lambert, 1.0);
	specularOutput = vec4(attenuated * specFactor * 0.33, 1.0);
}
