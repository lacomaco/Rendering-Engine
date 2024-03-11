#define MAX_LIGHTS 5
#define MAX_SPOT_LIGHT 2
#define MAX_POINT_LIGHT 2
#define Epsilon 0.00001
#define PI 3.141592

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 invTranspose;


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

uniform vec3 cameraPos;

// TODO: 지우기.
uniform float metallicTest;
uniform float roughnessTest;

