#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;
uniform sampler2D shadowTex;

uniform mat4 shadowMatrix;
uniform mat4 inverseProjView;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform vec4 lightPos;
uniform float lightRadius;
uniform vec4 lightColour;
uniform vec3 lightDir;
uniform float lightAngle;

uniform vec3 ambienceColour;

out vec4 ambienceOutput;
out vec4 diffuseOutput;
out vec4 specularOutput;

void main() {
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texture(depthTex, texCoord).r;
	if (depth == 1.0)
		discard;
	vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz / invClipPos.w;

	vec3 incident;
	float atten;
	if (lightPos.w == 0) {
		incident = normalize(lightDir);
		atten = 1.0;
	} else {
		vec3 lPos = lightPos.xyz / lightPos.w;
		incident = normalize(lPos - worldPos);
		if (lightAngle < 360.0 && degrees(acos(dot(-incident, lightDir))) > lightAngle)
			discard;
		float dist = length(lPos - worldPos);
		atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);
		if (atten == 0)
			discard;
	}

	vec3 normal = normalize(texture(normTex, texCoord).xyz * 2.0 - 1.0);
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	vec4 pushVal = vec4(normal, 0.0) * dot(viewDir, normal) * 100.0;
	vec4 shadowProj = shadowMatrix * (vec4(worldPos, 1) + pushVal);

	float shadow = 1.0;
	vec3 shadowNDC = shadowProj.xyz / shadowProj.w;
	if (abs(shadowNDC.x) < 1.0 && abs(shadowNDC.y) < 1.0 && abs(shadowNDC.z) < 1.0) {
		vec3 biasCoord = shadowNDC * 0.5 + 0.5;
		float shadowZ = texture(shadowTex, biasCoord.xy).x;
		shadow = (shadowZ < biasCoord.z) ? 0.0 : 1.0;
	}

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float rFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
	float specFactor = pow(rFactor, 60.0);

	vec3 attenuated = lightColour.xyz * atten;

	ambienceOutput = vec4(ambienceColour, 1.0);
	diffuseOutput = vec4(attenuated * lambert * shadow, 1.0);
	specularOutput = vec4(attenuated * specFactor * 0.33 * shadow, 1.0);
}
