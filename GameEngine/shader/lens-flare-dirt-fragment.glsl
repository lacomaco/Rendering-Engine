#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D bloomTexture;
uniform sampler2D dirtLensTexture;

void main() {
    vec3 color = texture(bloomTexture, TexCoords).rgb;
	vec3 lensDirt = texture(dirtLensTexture, TexCoords).rgb;
	FragColor = vec4(color * lensDirt, 1.0);
}
