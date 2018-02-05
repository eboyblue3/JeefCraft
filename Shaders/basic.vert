#version 120

attribute vec4 position;
varying vec3 vNormal;

uniform mat4 projViewMatrix;
uniform mat4 modelMatrix;

void main() {
	vec3 cNormals[6];
	cNormals[0] = vec3(1,0,0);  // East
	cNormals[1] = vec3(0,1,0);  // North
	cNormals[2] = vec3(-1,0,0); // West
	cNormals[3] = vec3(0,-1,0); // South
	cNormals[4] = vec3(0,0,1);  // Up
	cNormals[5] = vec3(0,0,-1); // Down

	mat4 mvp = projViewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position.xyz, 1.0);
	vNormal = cNormals[int(position.w)];
}