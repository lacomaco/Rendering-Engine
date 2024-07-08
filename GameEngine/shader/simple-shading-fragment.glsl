#version 460 core
#include common.glsl
// simple shading!
/*
�ּ����� �ڵ�� ���� �����ϰ� ������ ������ ���̴�.

�� ���̴� �ڵ�� 100% �Ϻ��ϰ� �������� �����Ѵ�.

���װ� �߻��ϸ� �� ���̴����� ���� �Ը�� �����ؼ� ���׸� ��������.
*/
out vec4 FragColor;

in vec3 tangent;
in vec3 normal;
in vec3 biTangent;
in mat3 TBN;
in vec2 TexCoords;
in vec3 FragPos;



void main() {
    mat3 TBN = mat3(
	    normalize(tangent),
		normalize(biTangent),
		normalize(normal)
	);

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

	FragColor = vec4(ambient + diffuse + specular, 1.0); // ������� �ƹ��� �̻� ����. ������ ���̴�! �׽�Ʈ�� �Ʒ����ٰ� ����
}

