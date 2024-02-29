#version 410 core
#include common.glsl

out vec4 FragColor;
in vec2 TexCoord;
in vec3 normalWorld;
in vec3 posWorld;


in vec4 directionalLightShadowSpace;
in vec4 spotLightShadowSpace[2];

void main() {

	vec3 toEye = normalize(cameraPos - posWorld);

	vec3 ambientColor = texture(albedo0,TexCoord).rgb;
	vec3 diffuseColor = texture(albedo0,TexCoord).rgb;
	vec3 specularColor = texture(specular0,TexCoord).rgb;

	vec3 color = vec3(0.0);

	int spotLightCount = 0;

	for(int i = 0; i < lightCount; i++){
	    Light light = lights[i];
		float shadow = 0.0;

		if(light.lightType == 0){

			shadow = shadowCalculation(
				directionalLightShadowSpace,
				directionalShadowDepthMap,
				normalWorld,
				normalize(light.position - posWorld)
			);

			color += directionalLight(
				light,
				material,
				posWorld,
				normalWorld,
				toEye,
				shadow,
				ambientColor,
				diffuseColor,
				specularColor
			);
		}
		else if(light.lightType == 2){

			shadow = shadowCalculation(
				spotLightShadowSpace[spotLightCount],
				spotShadowDepthMap[spotLightCount],
				normalWorld,
				normalize(light.position - posWorld)
			);

			spotLightCount++;

			color += spotLight(
				light,
				material,
				posWorld,
				normalWorld,
				toEye,
				shadow,
				ambientColor,
				diffuseColor,
				specularColor
			);
		}
	}
	/*
	for(int i = 0; i < pointLightCount; i++){
		float shadow = 0.0;
		Light light = pointLights[0];

		shadow = pointShadowCalculation(
			posWorld,
			light,
			pointShadowDepthMap[0]
		);


		color += pointLight(
			light,
			material,
			posWorld,
			normalWorld,
			toEye,
			shadow,
			ambientColor,
			diffuseColor,
			specularColor
		);
	}
	*/

	vec4 colorWithAlpha = vec4(color, texture(albedo0,TexCoord).a);

	FragColor = colorWithAlpha;
}
