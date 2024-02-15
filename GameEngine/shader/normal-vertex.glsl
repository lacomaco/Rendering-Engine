#version 330 core
#include common.glsl

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangentModel;
layout (location = 4) in vec3 aColor;

out VS_OUT {
	vec3 normal;
} vs_out;

void main() {
	gl_Position = vec4(aPos,1.0f);
	vs_out.normal = aNormal;
}
