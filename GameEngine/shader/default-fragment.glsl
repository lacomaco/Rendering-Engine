#version 460 core
#include common.glsl

out vec4 FragColor;
in vec2 TexCoord;
in vec3 normalWorld;
in vec3 posWorld;
in vec3 tangentWorld;


in vec4 directionalLightShadowSpace;
in vec4 spotLightShadowSpace[2];

void main() {

	vec3 toEye = normalize(cameraPos - posWorld);

	vec3 ambientColor = texture(albedo0,TexCoord).rgb;
	vec3 diffuseColor = texture(albedo0,TexCoord).rgb;
	vec3 specularColor = texture(specular0,TexCoord).rgb;

	vec3 color = vec3(0.0);

	int spotLightCount = 0;

	vec3 normal = normalWorld;

	if(use_normal0){
	    normal = getNormal(normalWorld, tangentWorld, normal0, TexCoord);
	}

	for(int i = 0; i < lightCount; i++){
	    Light light = lights[i];
		float shadow = 0.0;

		if(light.lightType == 0){

			shadow = shadowCalculation(
				directionalLightShadowSpace,
				directionalShadowDepthMap,
				normal,
				normalize(light.position - posWorld)
			);

			color += directionalLight(
				light,
				material,
				posWorld,
				normal,
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
				normal,
				normalize(light.position - posWorld)
			);

			spotLightCount++;

			color += spotLight(
				light,
				material,
				posWorld,
				normal,
				toEye,
				shadow,
				ambientColor,
				diffuseColor,
				specularColor
			);
		}
	}
	
	for(int i = 0; i < pointLightCount; i++) {
		float shadow = 0.0;
		Light light = pointLights[i]; // 현재 조명

	    shadow = pointShadowCalculation(
		    posWorld,
			light,
			pointShadowDepthMap[i]
		);

		color += pointLight(
			light,
			material,
			posWorld,
			normal,
			toEye,
			shadow,
			ambientColor,
			diffuseColor,
			specularColor
		);
	}
	

	vec4 colorWithAlpha = vec4(color, texture(albedo0,TexCoord).a);

	FragColor = colorWithAlpha;
}
