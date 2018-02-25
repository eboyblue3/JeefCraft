#version 120

attribute vec4 position;

uniform mat4 projViewMatrix;
uniform mat4 modelMatrix;

void main() {
	mat4 mvp = projViewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position.xyz, 1.0);
}