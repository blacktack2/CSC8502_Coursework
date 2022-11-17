#version 330 core

uniform mat4 modelMatrix;
uniform mat4 projViewMatrix;

in vec3 position;

void main() {
	gl_Position = projViewMatrix * modelMatrix * vec4(position, 1.0);
}
