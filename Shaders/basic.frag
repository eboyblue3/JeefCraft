#version 120

varying vec3 vNormal;
varying vec3 pos;

void main() {
	gl_FragColor = vec4((floor(pos) / vec3(16, 256, 16)), 1);
}