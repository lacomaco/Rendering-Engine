#version 330 core
#include common.glsl

out vec4 FragColor;
in vec2 TexCoord;
in vec3 normalWorld;
in vec3 posWorld;


void main() {
	vec3 toEye = normalize(cameraPos - posWorld);
	vec3 ambientColor = texture(albedo0,TexCoord).rgb;
	vec3 diffuseColor = texture(albedo0,TexCoord).rgb;
	vec3 specularColor = texture(specular0,TexCoord).rgb;

	vec3 color = pointLight(
	    light,
		material,
		posWorld,
		normalWorld,
		toEye,
		ambientColor,
		diffuseColor,
		specularColor
	);

	FragColor = vec4(color, 1.0);
}
