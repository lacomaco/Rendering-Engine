#version 460 core
#include common.glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangentModel;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoord;
out vec3 posWorld;
out vec3 normalWorld;
out vec4 directionalLightShadowSpace;
out vec4 spotLightShadowSpace[2];
out mat3 TBN;

void main() {

	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	posWorld = (model * vec4(aPos, 1.0f)).xyz;

	// normal은 법선벡터이기 때문에 0.0으로 설정.
	// 4차원이 0이면 이동 변환이 적용되지 않기 때문에 법선벡터는 0.0이다.
	normalWorld = normalize(vec3(model * vec4(aNormal,0.0f)));

	vec3 tangentWorld = aTangentModel;
	
	if(dot(cross(aNormal, aTangentModel), aBitangent) < 0.0) {
		tangentWorld = -aTangentModel;
	}

	tangentWorld = normalize(vec3(model * vec4(tangentWorld,0.0)));
	vec3 bitangentWorld = normalize(vec3(model * vec4(aBitangent,0.0)));

	TBN = mat3(tangentWorld, bitangentWorld, normalWorld);

	directionalLightShadowSpace = directionalShadowMap.lightSpaceMatrix * vec4(posWorld, 1.0);

	for(int i = 0; i < 2; i++){
		if(spotShadowMap[i].use){
			spotLightShadowSpace[i] = spotShadowMap[i].lightSpaceMatrix * vec4(posWorld, 1.0);
		}
		else {
			spotLightShadowSpace[i] = vec4(0.0);
		}
	}

	TexCoord = aTexCoord;
}