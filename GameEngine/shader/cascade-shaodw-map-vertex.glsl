#version 460 core
#include common.glsl
layout (location = 0) in vec3 aPos;

out int layerIndex;


void main() {
	layerIndex = gl_InstanceID;
	
	mat4 lightSpaceMatrix = lightSpaceMatrices[gl_InstanceID];
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}