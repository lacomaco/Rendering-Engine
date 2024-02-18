#version 330 core
#include common.glsl
#include linearize-depth.glsl

out vec4 FragColor;
in vec2 TexCoord;
in vec3 normalWorld;
in vec3 posWorld;


void main() {
	vec3 toEye = normalize(cameraPos - posWorld);
	vec3 ambientColor = texture(albedo0,TexCoord).rgb;
	vec3 diffuseColor = texture(albedo0,TexCoord).rgb;
	vec3 specularColor = texture(specular0,TexCoord).rgb;

	vec3 color = vec3(0.0);

	for(int i = 0; i < activeLight; i++){
	    Light light = lights[i];

		if(light.lightType == 0){
			color += directionalLight(
				light,
				material,
				posWorld,
				normalWorld,
				toEye,
				ambientColor,
				diffuseColor,
				specularColor
			);
		}
		else if(light.lightType == 1){
			color += pointLight(
				light,
				material,
				posWorld,
				normalWorld,
				toEye,
				ambientColor,
				diffuseColor,
				specularColor
			);
		}
		else if(light.lightType == 2){
			color += spotLight(
				light,
				material,
				posWorld,
				normalWorld,
				toEye,
				ambientColor,
				diffuseColor,
				specularColor
			);
		}
	}

	FragColor = vec4(color, 1.0);
}
