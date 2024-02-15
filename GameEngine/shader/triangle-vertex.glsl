#version 330 core
#include common.glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangentModel;
layout (location = 4) in vec3 aColor;

out vec2 TexCoord;
out vec3 posWorld;
out vec3 normalWorld;

void main() {
	// 위치벡터이기 때문에 1.0으로 설정
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	posWorld = (model * vec4(aPos, 1.0f)).xyz;

	// normal은 법선벡터이기 때문에 0.0으로 설정.
	// 4차원이 0이면 이동 변환이 적용되지 않기 때문에 법선벡터는 0.0이다.
	normalWorld = (invTranspose * vec4(aNormal,0.0f)).xyz;
	TexCoord = aTexCoord;
}