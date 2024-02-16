#version 330 core
#include common.glsl
out vec4 FragColor;

in vec2 TexCoord;
in vec3 normalWorld;
in vec3 posWorld;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * light.strength;

	vec3 lightDir = normalize(posWorld - light.position);
	float diff = max(dot(normalWorld, -lightDir), 0.0);
	vec3 diffuse = diff * light.strength;

	vec3 viewDir = normalize(cameraPos - posWorld);
	vec3 reflectDir = reflect(lightDir, normalWorld);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 specular = spec * light.specularStrength * light.strength;

	vec3 result = (diffuse + ambient + specular) * texture(albedo0,TexCoord).rgb;

	FragColor = vec4(result, 1.0);
}
