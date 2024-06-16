#version 460 core
#include common.glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangentModel;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoord;
out vec3 posWorld;

out vec3 tangentWorld;
out vec3 bitangentWorld;
out vec3 normalWorld;

out vec4 spotLightShadowSpace[2];

void main() {

	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	posWorld = vec3(model * vec4(aPos, 1.0f));

	normalWorld = invTranspose * aNormal;

	tangentWorld = invTranspose * aTangentModel;
	bitangentWorld = invTranspose * aBitangent;
	
	if(dot(cross(normalWorld, tangentWorld), bitangentWorld) < 0.0) {
		tangentWorld = tangentWorld * -1.0;
		bitangentWorld = cross(normalWorld, tangentWorld);
	}

	TexCoord = aTexCoord;
}