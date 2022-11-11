#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float lightRadius;
uniform vec4 lightPos;

in vec3 position;

void main() {
	if (lightPos.w == 0) {
		gl_Position = vec4(position, 1.0);
	} else {
		vec3 scale = vec3(lightRadius);
		vec3 worldPos = (vec4(position * scale, 1.0f) * modelMatrix).xyz + lightPos.xyz;
		gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1.0);
	}
}
