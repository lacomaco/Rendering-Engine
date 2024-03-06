#version 460 core
out vec3 FragColor;
in vec2 TexCoords;

uniform sampler2D srcTexture;
uniform float filterRadius;

void main(){
   float x = filterRadius;
   float y = filterRadius;

   // a - b - c
   // d - e - f
   // g - h - i

	vec3 a = texture(srcTexture, vec2(TexCoords.x - x, TexCoords.y + y)).rgb;
	vec3 b = texture(srcTexture, vec2(TexCoords.x, TexCoords.y + y)).rgb;
	vec3 c = texture(srcTexture, vec2(TexCoords.x + x, TexCoords.y + y)).rgb;

	vec3 d = texture(srcTexture, vec2(TexCoords.x - x, TexCoords.y)).rgb;
	vec3 e = texture(srcTexture, vec2(TexCoords.x, TexCoords.y)).rgb;
	vec3 f = texture(srcTexture, vec2(TexCoords.x + x, TexCoords.y)).rgb;

	vec3 g = texture(srcTexture, vec2(TexCoords.x - x, TexCoords.y - y)).rgb;
	vec3 h = texture(srcTexture, vec2(TexCoords.x, TexCoords.y - y)).rgb;
	vec3 i = texture(srcTexture, vec2(TexCoords.x + x, TexCoords.y - y)).rgb;

	// 1	|	1	2	1	|
	// --	|	2	4	2	|
	// 16	|	1	2	1	|

	FragColor = e * 4.0;
	FragColor += (b + d + f + h) * 2.0;
	FragColor += (a + c + g + i);
	FragColor /= 16.0;
}