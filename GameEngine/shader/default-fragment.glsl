#version 460 core
precision highp float;
#include common.glsl

uniform sampler2DArray shadowMaps;

out vec4 FragColor;

uniform float exposure;

in vec2 TexCoord;
in vec3 normalWorld;
in vec3 tangentWorld;
in vec3 bitangentWorld;
in vec3 posWorld;

in vec4 spotLightShadowSpace[2];

#include pbr.glsl

void main() {
    mat3 TBN = mat3(
		normalize(tangentWorld),
		normalize(bitangentWorld),
	    normalize(normalWorld)
	);

	vec3 toEye = normalize(cameraPos - posWorld);

	vec3 color = vec3(0.0);

	int spotLightCount = 0;

	vec3 normal = normalWorld;

	if(use_normal0){
	    normal = getNormal(normal0, TexCoord, TBN);
	}

	PBRValue pbrMaterial;

	vec3 pixelToEye = normalize(cameraPos - posWorld);

	vec3 ambientColor = pow(texture(albedo0,TexCoord).rgb,vec3(2.2));
	float roughness = use_roughness0 ? texture(roughness0, TexCoord).g : 0.0;
	float metallic = use_metallic0 ? texture(metallic0, TexCoord).b : 0.0;

	float ao = 1.0;

	if(use_ao0){
	    ao = texture(ao0, TexCoord).r;
	} else if (use_normal0) {
		vec3 normalTextureValue = texture(normal0, TexCoord).rgb;
		normalTextureValue = normalize(normalTextureValue * 2.0 - 1.0);
		ao = normalTextureValue.y * 0.5 + 0.5;
	}

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
			bool useCascadeDebug = false;

			if(useCascadeDebug){
				if(shadow.layer == 0){
					FragColor = vec4(1.0,0.0,0.0,1.0);
					return;
				}

				if(shadow.layer == 1){
					FragColor = vec4(0.0,1.0,0.0,1.0);
					return;
				}

				if(shadow.layer == 2){
					FragColor = vec4(0.0,0.0,1.0,1.0);
					return;
				}

				if(shadow.layer == 3){
					FragColor = vec4(1.0,1.0,0.0,1.0);
					return;
				}

				if(shadow.layer == 4){
					FragColor = vec4(1.0,0.0,1.0,1.0);
					return;
				}
			}

			color += directionalLight(
				light,
				shadow.shadow,
				pbrMaterial
			) * light.strength;

			shadowIndex += directionalCascadeLevel;
		} else if(light.lightType == 1){
		/*
			ShadowStruct shadow = pointShadowCalculation(
				posWorld,
				light,
				pointShadowDepthMap[i]
			);
		
			color += pointLight(
				light,
				shadow.shadow,
				pbrMaterial
			);
			*/
		}
		else if(light.lightType == 2){

		/*
			ShadowStruct shadow = shadowCalculation(
				spotLightShadowSpace[spotLightCount],
				spotShadowDepthMap[spotLightCount],
				normal,
				normalize(light.position - posWorld)
			);
		
			spotLightCount++;

			color += spotLight(
				light,
				shadow.shadow,
				pbrMaterial
			);
			*/
		}
	}

	vec3 ambientLight = ambientIBL(ambientColor, normal, pixelToEye, ao, metallic, roughness);
	color += ambientLight;
	vec4 colorWithAlpha = vec4(color, texture(albedo0,TexCoord).a);

	if(use_emissive0){
		colorWithAlpha += vec4(texture(emissive0, TexCoord).r) * 5.0;
	}

	FragColor = colorWithAlpha;
}

