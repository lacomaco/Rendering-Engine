#version 330 core
#include common.glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangentModel;
layout (location = 4) in vec3 aColor;

out vec2 TexCoord;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	TexCoord = aTexCoord;
}