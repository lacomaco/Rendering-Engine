#define MAX_LIGHTS 5
#define MAX_SPOT_LIGHT 2
#define MAX_POINT_LIGHT 2
#define Epsilon 0.00001
#define PI 3.141592

layout (std140, binding = 0) uniform Camera {
    mat4 projection; //64
    mat4 view; // 64
    mat4 skyBoxView; // 64
    vec3 cameraPos; //16 total : 208
    float padding; // 212
};

uniform mat4 model;

uniform mat3 invTranspose;


uniform sampler2D albedo0;

uniform bool use_metallic0;
uniform sampler2D metallic0;


uniform bool use_roughness0;
uniform sampler2D roughness0;


uniform bool use_ao0;
uniform sampler2D ao0;

uniform bool use_normal0;
uniform sampler2D normal0;

uniform bool use_height0;
uniform sampler2D height0;

uniform bool use_emissive0;
uniform sampler2D emissive0;


uniform samplerCube skyBox;
uniform samplerCube radianceMap; //specular
uniform samplerCube irradianceMap;
uniform sampler2D brdfTexture;

uniform float far;

struct NormalShadowMap {
    bool use;
    mat4 lightSpaceMatrix;
};

uniform NormalShadowMap directionalShadowMap;
uniform sampler2D directionalShadowDepthMap;

uniform NormalShadowMap spotShadowMap[MAX_SPOT_LIGHT];
uniform sampler2D spotShadowDepthMap[MAX_SPOT_LIGHT];

uniform samplerCube pointShadowDepthMap[MAX_POINT_LIGHT];

struct Material {
    // phong shading ����.
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;

    vec3 fresnelIRO;
};

uniform Material material;

uniform int lightCount;
uniform int pointLightCount;

struct Light {
	vec3 direction; // Directional Light ������ �����.
    vec3 position;
    int lightType; // 0: directional, 1: point, 2: spot

    vec3 strength; // ���� ����

    // ������
    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float cutOuter;
};

uniform Light lights[MAX_LIGHTS - 2];

uniform Light pointLights[2];

// TODO: �����.
uniform float metallicTest;
uniform float roughnessTest;


/*
TMI

nomalMap�� �������� ����� 2���� ����� �ִ�.

1. �Ʒ� �����Ȱ�ó�� normalMap�� ���ø��� �� TBN ����� ����Ͽ� ���� ��ǥ��� ��ȯ�ϴ¹��.

2. ī�޶�, ���� ������ TBN ����� "�����"�� ����Ͽ� ���� �����̽� -> Tangent Space�� �Űܼ� ����ϴ� ���.

�� ��� normalMap�� Tangent Space�� �־�� ������ TBN ����� ������ ���ƾ��Ѵ�.

2�� ����� ��� vertex shader���� ī�޶�,�� ������ TBN ������ �ϸ� �Ǳ� ������ ȿ����������. ���������� ����.

1�� ����� ��� fragment shader���� TBN ����� ����ϱ� ������ ��ȿ���������� ��������.

���� 1�� ����� ����Ͽ���.


solution: https://stackoverflow.com/questions/47620285/normal-mapping-bug
*/
vec3 getNormal(sampler2D normalMap,vec2 TexCoords,mat3 TBN){
	vec3 n = texture(normalMap, TexCoords).rgb;
	n = n * 2.0 - 1.0; // [-1 ~ 1] ����ȭ.
	return normalize(TBN * n);
}
