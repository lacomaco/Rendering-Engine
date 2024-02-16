#version 330 core
#include common.glsl
out vec4 FragColor;

in vec2 TexCoord;
in vec3 normalWorld;
in vec3 posWorld;

void main() {
	vec3 ambient = light.ambient * texture(albedo0,TexCoord).rgb;

	vec3 lightDir = normalize(posWorld - light.position);
	float diff = max(dot(normalWorld, -lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * texture(albedo0, TexCoord).rgb;

	vec3 viewDir = normalize(cameraPos - posWorld);
	vec3 reflectDir = reflect(lightDir, normalWorld);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 specular = spec * texture(specular0,TexCoord).rgb * light.specular;

	vec3 result = diffuse + ambient + specular;

	FragColor = vec4(result, 1.0);
}
