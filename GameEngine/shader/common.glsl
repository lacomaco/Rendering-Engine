#define MAX_LIGHTS 100
// 그림자 가능한 빛은 directional 1, point 5, spot 4개로 한정짓는다.
#define MAX_SPOT_LIGHT 4 // 나중에 지울거임
#define MAX_POINT_LIGHT 5 // 나중에 지울거임
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

// 빛은 그림자가 있는 directional, point spot이 먼저 들어온다.
struct Light {
        vec3 direction;
        bool isShadowLight;

        vec3 position;
        int lightType;

        vec3 strength;
        float constant;

        float linear;
        float quadratic;
        float cutOff;
        float cutOuter;
        // total 64 byte
};

struct Shadow {
    mat4 lightMatrices[6]; // 94
    int lightIndex;
    int matrixCount; // directional: 3, Point: 6, Spot: 2
    float padding2[2];

    // 98 byte
};

layout (std140, binding = 1) uniform Lights {
    Light lights[MAX_LIGHTS]; // 6400 byte

    int lightCount; // active Light Count
    int directionalCascadeLevel;
    vec2 padding3;

    // all 6416 byte
};

layout (std140, binding = 2) uniform Shadows {
    Shadow shadows[10];
    int shadowCount;
    vec3 padding4;

    // 84 byte
};

// 그림자는
// directional point spot 순으로 들어온다.
uniform sampler2DArray ShadowAtlas;

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


struct Material {
    // phong shading 전용.
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;

    vec3 fresnelIRO;
};

uniform Material material;


// TODO: 지우기.
uniform float metallicTest;
uniform float roughnessTest;


vec3 getNormal(sampler2D normalMap,vec2 TexCoords,mat3 TBN){
	vec3 n = texture(normalMap, TexCoords).rgb;
	n = n * 2.0 - 1.0; // [-1 ~ 1] 정규화.
	return normalize(TBN * n);
}
