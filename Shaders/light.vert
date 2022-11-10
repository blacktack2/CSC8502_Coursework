#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec4 lightPos;
uniform vec4 lightColour;

in vec3 position;

void main() {
	vec3 scale = vec3(lightRadius);
	vec3 worldPos = (position * scale);
	worldPos += lightPos.w == 0 ? cameraPos : lightPos.xyz;
	gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1.0);
}
