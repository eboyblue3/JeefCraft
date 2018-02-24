#version 120

varying vec3 vNormal;
varying vec3 pos;
varying vec2 vUvs;

uniform sampler2D textureAtlas;

const vec3 sun_dir = vec3(0.32, 0.75, 0.54);
const vec3 sun_color = vec3(1.4, 1.2, 0.4);
const vec4 ambient = vec4(0.3, 0.3, 0.4, 0.0);

void main() {
	vec4 diffuse = texture2D(textureAtlas, vUvs);
	float cosTheta = clamp(dot(vNormal, sun_dir), 0.0, 1.0);
	vec4 sun_color_theta = vec4(sun_color * cosTheta, 1.0) + ambient;
	gl_FragColor = diffuse * sun_color_theta;
}