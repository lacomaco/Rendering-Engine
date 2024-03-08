#define MAX_LIGHTS 5
#define MAX_SPOT_LIGHT 2
#define MAX_POINT_LIGHT 2

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 invTranspose;


uniform sampler2D albedo0;
uniform sampler2D albedo1;
uniform sampler2D specular0;

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

vec3 schlickFresnel(vec3 iro, vec3 normal, vec3 toEye); // 전방선언

float calcAttenuation(float distance,Light l) {
	return 1.0 / distance;
}

// 나중에 uniform으로 바꿀 수 있도록 변경해도 괜찮을듯.
vec3 Fdielectric = vec3(0.04); // 비금속 재질의 F0

struct PBRValue {
	float ndotH; // dot(normalWorld, halfWayDir);
	float ndotl; // dot(normalWorld, toLightDirection);
	float ndotO; // dot(normalWorld, pixelToEye);
    vec3 halfWayDir;
	vec3 albedo;
	vec3 normalWorld;
	vec3 pixelToEye;
	float metallic;
	float roughness;
	float ao;
};

vec3 SchlickFresnel(vec3 F0, float ndotH){
    return F0 + (1.0 - F0) *  pow(2.0, (-5.55473 * ndotH - 6.98316) * ndotH);
}

vec3 diffuseIBL(vec3 albedo,vec3 normalWorld,vec3 pixelToEye,float metallic) {
    vec3 F0 = mix(Fdielectric, albedo, metallic);
    vec3 F = SchlickFresnel(F0, max(dot(normalWorld,pixelToEye),0.0));
    vec3 kd = mix(1.0 - F, vec3(0.0), metallic);

    vec3 irradiance = texture(irradianceMap, normalWorld).rgb;

    return kd * albedo;
}

vec3 specularIBL(vec3 albedo, vec3 normalWorld, vec3 pixelToEye, float metallic,float roughness) {
    vec2 brdf = texture(
    brdfTexture, 
    vec2(dot(pixelToEye,normalWorld),1.0 - roughness)
    ).rg;

    vec3 specularIrradiance = textureLod(radianceMap, reflect(-pixelToEye,normalWorld), roughness * 6.0f).rgb;
    vec3 F0 = mix(Fdielectric, albedo, metallic);

    return (F0 * brdf.x  + brdf.y) * specularIrradiance;
}


vec3 ambientIBL(vec3 albedo, vec3 normalW, vec3 pixelToEye, float ao, float metallic,float roughness){
    vec3 diffuseIBL = diffuseIBL(albedo,normalW,pixelToEye,metallic);
    vec3 specularIBL = specularIBL(albedo,normalW,pixelToEye,metallic,roughness);

    return (diffuseIBL + specularIBL) * ao;
}

float NdfGGX(float ndotH, float roughness) {
    float pi = 3.141592;
    return pow(roughness, 2) / (pi * pow(pow(ndotH, 2) * (pow(roughness, 2) - 1) + 1, 2));
}

float SchlickG1(float ndotV, float k) {
	return ndotV / (ndotV * (1.0 - k) + k);
}

float SchlickGGX(float ndotI, float ndotO, float roughness) {
    float k = pow(roughness + 1, 2) / 8;
	return SchlickG1(ndotI, k) * SchlickG1(ndotO, k);
}

vec3 PBR(PBRValue value,vec3 lightStrength,float shadow){
    vec3 F0 = mix(Fdielectric, value.albedo, value.metallic);
    vec3 F = SchlickFresnel(F0, max(dot(value.halfWayDir,value.pixelToEye),0.0));
    vec3 kd = mix(1.0 - F, vec3(0.0), value.metallic);
    vec3 diffuseBRDF = kd * value.albedo;

    float D = NdfGGX(value.ndotH, value.roughness);
    float G = SchlickGGX(value.ndotl, value.ndotO, value.roughness);

    vec3 specularBRDF = (D * G * F) / max((4 * value.ndotl * value.ndotO),0.00001);
    vec3 radiance = lightStrength * value.ndotl;

    return (diffuseBRDF + specularBRDF) * radiance * (1.0 - shadow);
}

vec3 directionalLight(
    Light l, 
    float shadow,
    PBRValue value
    ) {

    // 빛 -> 물체가 아닌 물체 -> 빛 방향임.
    vec3 lightVec = normalize(-l.direction);

    vec3 halfWayDir = normalize(lightVec + value.pixelToEye);

    value.ndotH = max(dot(value.normalWorld, halfWayDir), 0.0);
    value.ndotl = max(dot(value.normalWorld, lightVec), 0.0);
    value.halfWayDir = halfWayDir;

    vec3 lightStrength = vec3(max(dot(value.normalWorld, lightVec), 0.0));

    return PBR(value,lightStrength,shadow);
}

vec3 pointLight(
    Light l,
    float shadow,
    PBRValue value
) {
	vec3 toLight = normalize(l.position - value.pixelToEye);
    float lightStrength = max(dot(value.normalWorld, toLight), 0.0);
    float distance = length(l.position - value.pixelToEye);
    vec3 attenuation = vec3(calcAttenuation(distance,l));

    // ndotH, ndotL, halfWayDir

    vec3 halfWaydir = normalize(toLight + value.pixelToEye);
    float ndotH = max(dot(value.normalWorld,halfWaydir),0.0);
    float ndotl = max(dot(value.normalWorld,toLight),0.0);

    value.ndotH = ndotH;
    value.ndotl = ndotl;
    value.halfWayDir = halfWaydir;

    return PBR(value,attenuation,shadow);
}

vec3 spotLight(    
    Light l,
    float shadow,
    PBRValue value
    ) {
    vec3 lightVec = normalize(l.position - value.pixelToEye);

    float theta = dot(lightVec, normalize(-l.direction));

    // 용어 정리겸 Intensity 공식 정리
    // Phi = 스포트라이트를 제대로 받는 각도 (cutOff)
    // Theta = LightDirection과 LightVec의 각도 쉽게 말해 현재 픽셀과 스포트라이트 방향의 각도이다.
    // gamma = 외부 각도, 넘어가면 외부 조명 받지 않음 (cutOuter)
    // epsilon = 외부 각도와 내부 각도 사이의 각도 cutOff - cutOuter "음수값이다."
    // Intensity = (Theta - cutOuter) / (cutOff - cutOuter)
    // Theta가 Phi랑 같다면 (스포트라이트의 경계면) Intensity = 1
    // Theta가 감마보다 크다면 음수가 되어 clamp에 의해 Intensity = 0;
    // Theta가 Phi보다 Phi보다 작다면 Intensity는 양수 1보다 큰 값이 되어 clamp에 의해 1이된다.
    // Theta가 Phi보다 크고 Theta보다 작으면 비로소 0~1 사이의 값이 나온다.

    float epsilon = l.cutOff - l.cutOuter;
    // as-is intensity 최대값 1
    // to-be intensity 최대값 = 5
    // 최대값이 1이면 스포트라이트가 너무 약하게 나와 한계를 늘려주었음.
    float intensity = clamp((theta - l.cutOuter) / epsilon, 0.1, 5.0);

    float distance = length(l.position - value.pixelToEye);
    float attenuation = calcAttenuation(distance,l);

    vec3 lightStrength = vec3(attenuation * intensity);

    // ndotH, ndotL, halfWayDir
    vec3 halfWaydir = normalize(lightVec + value.pixelToEye);
    float ndotH = max(dot(value.normalWorld,halfWaydir),0.0);
    float ndotl = max(dot(value.normalWorld,lightVec),0.0);

    value.ndotH = ndotH;
	value.ndotl = ndotl;
    value.halfWayDir = halfWaydir;

    return PBR(value,lightStrength,shadow);
}

// RGB를 HSV로 변환하는 함수
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0/3.0, 2.0/3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// HSV를 RGB로 변환하는 함수
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float shadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap,vec3 normal, vec3 lightDir) {
    float bias = 0.001;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // z값을 조금 높여 색을 가져온다. 큐브와 바닥이 바짝 붙어있으면 그림자가 생기지 않는 이슈가 있다.
    //projCoords.z = projCoords.z + 0.01;
    //projCoords.y = projCoords.y + 0.0001;

    projCoords += lightDir * 0.0001;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    const int halfkernelWidth = 2;

    for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x) {
        for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return shadow / ((halfkernelWidth * 2 + 1) * (halfkernelWidth * 2 + 1));
}

float pointShadowCalculation(vec3 posWorld,Light light,samplerCube map){
    vec3 posToLight = posWorld - light.position;

    float closestDepth = texture(map, posToLight).r;

    closestDepth *= far;

    float currentDepth = length(posToLight);

    float bias = 0.05;

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

/*
https://psgraphics.blogspot.com/2020/03/fresnel-equations-schlick-approximation.html

vec3 R0 = (guess by looking)
vec3 R = R0 + (1-R0)*pow(1-cosine,5)
*/
vec3 schlickFresnel(vec3 iro, vec3 normal, vec3 toEye) {
    float lookAngle = dot(normal,toEye);
    // 90도에 가까우면 본연의 색을, 0도에 가까우면 반사색을 반환.
    lookAngle = 1.0 - clamp(lookAngle,0.0,1.0);

    return iro + (1.0f - iro) * pow(lookAngle,5.0);
}

