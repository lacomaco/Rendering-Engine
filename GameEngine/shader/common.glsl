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
    // phong shading 전용.
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
	vec3 direction; // Directional Light 에서만 사용함.
    vec3 position;
    int lightType; // 0: directional, 1: point, 2: spot

    vec3 strength; // 빛의 세기

    // 빛감쇠
    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float cutOuter;
};

uniform Light lights[MAX_LIGHTS - 2];

uniform Light pointLights[2];

// TODO: 지우기.
uniform float metallicTest;
uniform float roughnessTest;


/*
TMI

nomalMap을 가져오는 방법엔 2가지 방법이 있다.

1. 아래 구현된것처럼 normalMap을 샘플링한 후 TBN 행렬을 사용하여 월드 좌표계로 변환하는방법.

2. 카메라, 빛의 방향을 TBN 행렬의 "역행렬"을 사용하여 월드 스페이스 -> Tangent Space로 옮겨서 계산하는 방법.

이 경우 normalMap은 Tangent Space에 있어야 함으로 TBN 행렬을 곱하지 말아야한다.

2번 방법의 경우 vertex shader에서 카메라,빛 방향을 TBN 연산을 하면 되기 때문에 효율적이지만. 직관적이지 않음.

1번 방법의 경우 fragment shader에서 TBN 행렬을 사용하기 때문에 비효율적이지만 직관적임.

나는 1번 방법을 사용하엿음.


solution: https://stackoverflow.com/questions/47620285/normal-mapping-bug
*/
vec3 getNormal(sampler2D normalMap,vec2 TexCoords,mat3 TBN){
	vec3 n = texture(normalMap, TexCoords).rgb;
	n = n * 2.0 - 1.0; // [-1 ~ 1] 정규화.
	return normalize(TBN * n);
}
