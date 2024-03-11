#version 460 core
#include common.glsl
// simple shading!
/*
최소한의 코드로 가장 간단하게 구현한 라이팅 쉐이더.

이 쉐이더 코드는 100% 완벽하게 동작함을 보장한다.

버그가 발생하면 이 쉐이더에서 작은 규모로 재현해서 버그를 수정하자.

이 쉐이더는 완벽 그자체다. 의심하지말자.
*/
out vec4 FragColor;

in vec3 tangent;
in vec3 normal;
in vec3 biTangent;
in mat3 TBN;
in vec2 TexCoords;
in vec3 FragPos;



void main() {
    vec3 n = texture(normal0, TexCoords).rgb;
	n = n * 2.0 - 1.0;

	vec3 cnormal = normalize(TBN * n);

	vec3 color = texture(albedo0,TexCoords).rgb;

	vec3 ambient = 0.1 * color;

	vec3 lightPos = lights[0].position;


	vec3 lightDir = normalize(lightPos - FragPos);
	
	vec3 directionalLightDir = -lights[0].direction;
	float lightStrength = max(dot(cnormal, directionalLightDir), 0.0);
	vec3 diffuse = lightStrength * color;
	
	vec3 viewPos = cameraPos;

	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 reflectDir = reflect(-lightDir, cnormal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(cnormal, halfwayDir), 0.0), 32.0);

	vec3 specular = vec3(0.2) * spec;

	FragColor = vec4(ambient + diffuse + specular, 1.0); // 여기까지 아무런 이상 없음. 정상임 쉐이더! 테스트는 아래에다가 진행
}

