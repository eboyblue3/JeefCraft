#version 120

varying vec3 vNormal;
varying vec3 pos;
varying vec2 vUvs;

uniform sampler2D textureAtlas;

void main() {
	gl_FragColor = texture2D(textureAtlas, vUvs);
}