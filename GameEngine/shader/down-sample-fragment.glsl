#version 460 core
out vec3 FragColor;
in vec2 TexCoords;

uniform sampler2D srcTexture;
uniform vec2 resolution;

void main() {
   vec2 texelSize = 1.0 / resolution;
   float x = texelSize.x;
   float y = texelSize.y;

	// a - b - c
	// - j - k -
	// d - e - f
	// - l - m -
	// g - h - i

	vec3 a = texture(srcTexture, vec2(TexCoords.x - 2*x,	TexCoords.y + 2*y)).rgb;
	vec3 b = texture(srcTexture, vec2(TexCoords.x,			TexCoords.y + 2*y)).rgb;
	vec3 c = texture(srcTexture, vec2(TexCoords.x + 2*x,	TexCoords.y + 2*y)).rgb;

	vec3 d = texture(srcTexture, vec2(TexCoords.x - 2*x,	TexCoords.y)).rgb;
	vec3 e = texture(srcTexture, vec2(TexCoords.x,			TexCoords.y)).rgb;
	vec3 f = texture(srcTexture, vec2(TexCoords.x + 2*x,	TexCoords.y)).rgb;

	vec3 g = texture(srcTexture, vec2(TexCoords.x - 2*x,	TexCoords.y - 2*y)).rgb;
	vec3 h = texture(srcTexture, vec2(TexCoords.x,			TexCoords.y - 2*y)).rgb;
	vec3 i = texture(srcTexture, vec2(TexCoords.x + 2*x,	TexCoords.y - 2*y)).rgb;

	vec3 j = texture(srcTexture, vec2(TexCoords.x - x,		TexCoords.y + y)).rgb;
	vec3 k = texture(srcTexture, vec2(TexCoords.x + x,		TexCoords.y + y)).rgb;
	vec3 l = texture(srcTexture, vec2(TexCoords.x - x,		TexCoords.y - y)).rgb;
	vec3 m = texture(srcTexture, vec2(TexCoords.x + x,		TexCoords.y - y)).rgb;

	FragColor = e * 0.125;
	FragColor += (a + c + g + i) * 0.03125;
	FragColor += (b + d + f + h) * 0.0625;
	FragColor += (j + k + l + m) * 0.125;
}
