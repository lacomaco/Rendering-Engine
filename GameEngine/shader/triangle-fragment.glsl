#version 330 core
#include common.glsl
out vec4 FragColor;

in vec2 TexCoord;

void main() {
	FragColor = texture(albedo0,TexCoord);
}
