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

	for(int i = 0; i < lightCount; i++){
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

	vec4 colorWithAlpha = vec4(color, texture(albedo0,TexCoord).a);
	/*
	vec4 diffuse = texture(radianceMap, normalWorld);
	vec4 specular = texture(irradianceMap, reflect(-toEye,normalWorld));

	
	specular *= pow(
	    (specular.x + specular.y + specular.z)/3.0,
		material.shininess
	);

	diffuse *= vec4(material.diffuse,1.0);

	FragColor = colorWithAlpha * (diffuse + specular);
	*/

	FragColor = colorWithAlpha;

}
