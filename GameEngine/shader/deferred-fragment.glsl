#version 460 core
precision highp float;

uniform sampler2D positionMetallicTexture;
uniform sampler2D albedoRoughnessTexture;
uniform sampler2D normalTexture;

#include common.glsl

uniform sampler2DArray shadowMaps;

out vec4 FragColor;

in vec2 TexCoords;

#include SH-common.glsl
#include pbr.glsl

void main() {
	vec3 normal = texture(normalTexture, TexCoords).rgb;
	vec3 posWorld = texture(positionMetallicTexture, TexCoords).rgb;
	float roughness = texture(albedoRoughnessTexture, TexCoords).a;
	float metallic = texture(positionMetallicTexture, TexCoords).a;
	vec3 ambientColor = pow(texture(albedoRoughnessTexture,TexCoords).rgb,vec3(2.2));

	vec3 toEye = normalize(cameraPos - posWorld);

	vec3 color = vec3(0.0);

	int spotLightCount = 0;

	PBRValue pbrMaterial;

	vec3 pixelToEye = normalize(cameraPos - posWorld);

	float ao = 1.0;

	// halfWayVector 계산은 라이트 처리에서 다룸.
	float ndotO = max(dot(normal, pixelToEye), 0.0);

	pbrMaterial.albedo = ambientColor;
	pbrMaterial.metallic = metallic;
	pbrMaterial.roughness = roughness;
	pbrMaterial.ao = ao;
	pbrMaterial.normalWorld = normal;
	pbrMaterial.pixelToEye = pixelToEye;
	pbrMaterial.ndotO = ndotO;
	pbrMaterial.posWorld = posWorld;



	float attenuation = 0.0;

	int shadowMapPointer = 0;
	int shadowIndex = 0;

	for(int i = 0; i < lightCount; i++){
	    Light light = lights[i];
		if(light.lightType == 0){
			ShadowStruct shadow = cascadeShadowCalculation(
				posWorld,
				shadowIndex,
				light
			);

			color += directionalLight(
				light,
				shadow.shadow,
				pbrMaterial
			);

			shadowIndex += directionalCascadeLevel;
		}
	}

	vec3 ambientLight = ambientIBL(ambientColor, normal, pixelToEye, ao, metallic, roughness);
	color += ambientLight;

	FragColor = vec4(color,1.0);
}
