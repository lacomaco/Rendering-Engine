#version 330 core
#include common.glsl
out vec4 FragColor;

in vec2 TexCoord;

void main() {
	FragColor = mix(texture(albedo0, TexCoord), texture(albedo1, TexCoord), 0.2);
}
