#version 460 core
precision highp float;
#include common.glsl
#include pbr.glsl



out vec4 FragColor;

uniform float exposure;

in vec2 TexCoord;
in vec3 normalWorld;
in vec3 posWorld;
in mat3 TBN;

in vec4 directionalLightShadowSpace;
in vec4 spotLightShadowSpace[2];

vec3 getNormal(sampler2D normalMap,vec2 TexCoords);

Light getNearestLight(Light nearestLight, Light light,bool isShadow,bool isInit){
    if(!isInit){
	    return light;
	}

	if(isShadow){
	    return nearestLight;
	}

	vec3 pixelToEye = normalize(cameraPos - posWorld);
	float lightDistance = length(light.position - posWorld);
	float nearestLightDistance = length(nearestLight.position - posWorld);

	return lightDistance < nearestLightDistance ? light : nearestLight;
}

void main() {

	vec3 toEye = normalize(cameraPos - posWorld);

	vec3 color = vec3(0.0);

	int spotLightCount = 0;

	vec3 normal = normalWorld;

	if(use_normal0){
	    normal = getNormal(normal0, TexCoord);
	}

	PBRValue pbrMaterial;

	vec3 pixelToEye = normalize(cameraPos - posWorld);

	vec3 ambientColor = pow(texture(albedo0,TexCoord).rgb,vec3(2.2));
	/*
	조합
	g b (흠...)
	b r
	b g 
	b b
	*/
	float roughness = use_roughness0 ? texture(roughness0, TexCoord).g : 0.0;
	float metallic = use_metallic0 ? texture(metallic0, TexCoord).b : 0.0;

	float ao = use_ao0 ? texture(ao0, TexCoord).r : 1.0;

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
	// ndotH, ndotl, halfWay는 라이트처리에서 핸들링.
	// 라이트 처리에서 lightStrength도 개별적으로 처리해주어야함.

	// GI 기능이 아직 없어서 땜빵으로 구현한 코드 GI 기능 추가시 제거해야함.
	float countLight = 0.0;
	bool isInit = false;
	Light nearestLight;

	// 이 그림자가 태양 빛인지, 아닌지 구분해야함.
	bool isDirectionalShadow = false;
	bool isShadow = false;

	for(int i = 0; i < lightCount; i++){
	    Light light = lights[i];
		if(light.lightType == 0){

			ShadowStruct shadow = shadowCalculation(
				directionalLightShadowSpace,
				directionalShadowDepthMap,
				normal,
				-light.direction
			);

			countLight = 1.0 - shadow.shadow;

			// GI 기능이 아직 없어서 땜빵으로 구현한 코드 GI 기능 추가시 제거해야함.
			if(!shadow.isShadow){
				nearestLight = getNearestLight(nearestLight, light,shadow.isShadow, isInit);
				isInit = true;
			} else {
			    isDirectionalShadow = true;
			}

			color += directionalLight(
				light,
				shadow.shadow,
				pbrMaterial
			);
		}
		else if(light.lightType == 2){

			ShadowStruct shadow = shadowCalculation(
				spotLightShadowSpace[spotLightCount],
				spotShadowDepthMap[spotLightCount],
				normal,
				normalize(light.position - posWorld)
			);

			countLight = 1.0 - shadow.shadow;

			// GI 기능이 아직 없어서 땜빵으로 구현한 코드 GI 기능 추가시 제거해야함.
			if(!shadow.isShadow){
				nearestLight = getNearestLight(nearestLight, light, shadow.isShadow, isInit);
				isInit = true;
			} else {
			  isDirectionalShadow = false;
			}

			spotLightCount++;

			color += spotLight(
				light,
				shadow.shadow,
				pbrMaterial
			);
		}
	}
	
	for(int i = 0; i < pointLightCount; i++) {
		Light light = pointLights[i]; // 현재 조명

	    ShadowStruct shadow = pointShadowCalculation(
		    posWorld,
			light,
			pointShadowDepthMap[i]
		);

		// GI 기능이 아직 없어서 땜빵으로 구현한 코드 GI 기능 추가시 제거해야함.

		if(!shadow.isShadow){
			nearestLight = getNearestLight(nearestLight, light, shadow.isShadow,isInit);
			isInit = true;
		} else {
			isDirectionalShadow = false;
		}

		
		countLight = 1.0 - shadow.shadow;

		color += pointLight(
			light,
			shadow.shadow,
			pbrMaterial
		);
	}


	vec3 ambientLight = ambientIBL(ambientColor, normal, pixelToEye, ao, metallic, roughness, countLight, pbrMaterial.ndotl);

	if(countLight == 0.0){
	    ambientLight *= vec3(0.3);
	} else if(!isDirectionalShadow) {
		float distance = length(nearestLight.position - posWorld);
		float attenuation = calcAttenuation(distance,nearestLight);
		ambientLight *= attenuation;
	}

	color += ambientLight;
	vec4 colorWithAlpha = vec4(color, texture(albedo0,TexCoord).a);

	FragColor = colorWithAlpha;
}

/*
TMI

nomalMap을 가져오는 방법엔 2가지 방법이 있다.

1. 아래 구현된것처럼 normalMap을 샘플링한 후 TBN 행렬을 사용하여 월드 좌표계로 변환하는방법.

2. 카메라, 빛의 방향을 TBN 행렬의 "역행렬"을 사용하여 월드 스페이스 -> Tangent Space로 옮겨서 계산하는 방법.

이 경우 normalMap은 Tangent Space에 있어야 함으로 TBN 행렬을 곱하지 말아야한다.

2번 방법의 경우 vertex shader에서 카메라,빛 방향을 TBN 연산을 하면 되기 때문에 효율적이지만. 직관적이지 않음.

1번 방법의 경우 fragment shader에서 TBN 행렬을 사용하기 때문에 비효율적이지만 직관적임.

나는 1번 방법을 사용하엿음.


solution: https://stackoverflow.com/questions/47620285/normal-mapping-bug
*/
vec3 getNormal(sampler2D normalMap,vec2 TexCoords){
	vec3 n = texture(normalMap, TexCoords).rgb;
	n = n * 2.0 - 1.0; // [-1 ~ 1] 정규화.
	return normalize(TBN * n);
}
