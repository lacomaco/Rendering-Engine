#define MAX_LIGHTS 5

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 invTranspose;


uniform sampler2D albedo0;
uniform sampler2D specular0;


uniform samplerCube skyBox;
uniform samplerCube radianceMap;
uniform samplerCube irradianceMap;

struct NormalShadowMap {
	sampler2D depthMap;
    bool use;
    mat4 lightSpaceMatrix;
};

struct PointShadowMap {
	samplerCube depthMap;
    bool use;
	mat4 lightSpaceMatrix;
};

uniform NormalShadowMap directionalShadowMap;
uniform NormalShadowMap spotShadowMap[2];
uniform PointShadowMap pointShadowMap[2];

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

uniform Light lights[MAX_LIGHTS];

uniform vec3 cameraPos;

vec3 schlickFresnel(vec3 iro, vec3 normal, vec3 toEye); // 전방선언

float calcAttenuation(float distance,Light l) {
	return 1.0 / distance;
}

vec3 ambientIBL(Material material,vec3 normal, vec3 toEye) {
    vec4 diffuse = texture(radianceMap, normal);
    vec4 specular = texture(irradianceMap,reflect(-toEye,normal));

    specular *= pow(
        (specular.x + specular.y + specular.z) / 3.0,
        material.shininess
    );

    return (specular + diffuse).rgb;
}

vec3 phongShading(
    Light l,
    float lightStrength,
    vec3 toLightDirection, // 지표면 <- 빛 방향이다.
    vec3 normal,
    vec3 toEye,
    float shadow,
    Material mat,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor
    ) {
    vec3 halfWayDir = normalize(toLightDirection + toEye);

	vec3 ambient = ambientIBL(mat,normal,toEye) * mat.ambient * ambientColor;

    vec3 diffuse = lightStrength * mat.diffuse * diffuseColor * l.strength;

    float spec = pow(max(dot(normal, halfWayDir), 0.0), mat.shininess);

	vec3 specular = specularColor * mat.specular * l.strength * spec;

    vec3 f = schlickFresnel(mat.fresnelIRO,normal,toEye);

    specular *= f;

	return ambient + (diffuse + specular) * (1.0 - shadow);
}

vec3 directionalLight(
    Light l, 
    Material mat,
    vec3 posWorld,
    vec3 normal,
    vec3 toEye,
    float shadow,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor) {

    vec3 lightVec = normalize(-l.direction);

    float lightStrength = max(dot(normal, lightVec), 0.0);

    return phongShading(
        l,
        lightStrength,
        lightVec,normal,
        toEye,
        shadow,
        mat,
        ambientColor,
        diffuseColor,
        specularColor
    );
}

vec3 pointLight(
    Light l, 
    Material mat,
    vec3 posWorld,
    vec3 normal,
    vec3 toEye,
    float shadow,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor
) {
	vec3 toLight = normalize(l.position - posWorld);
    float lightStrength = max(dot(normal, toLight), 0.0);
    float distance = length(l.position - posWorld);
    float attenuation = calcAttenuation(distance,l);

    ambientColor *= attenuation;
    diffuseColor *= attenuation;
    specularColor *= attenuation;

    return phongShading(
        l,
        lightStrength,
        toLight,
        normal,
        toEye,
        shadow,
        mat,
        ambientColor,
        diffuseColor,
        specularColor
    );
}

vec3 spotLight(    
    Light l, 
    Material mat,
    vec3 posWorld,
    vec3 normal,
    vec3 toEye,
    float shadow,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor
    ) {
    vec3 lightVec = normalize(l.position - posWorld);

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
    float intensity = clamp((theta - l.cutOuter) / epsilon, mat.ambient.r, 5.0);

    float lightStrength = max(dot(normal, lightVec), 0.0);
    float distance = length(l.position - posWorld);
    float attenuation = calcAttenuation(distance,l);

    ambientColor *= attenuation * intensity;
    diffuseColor *= attenuation * intensity;
    specularColor *= attenuation * intensity;

    return phongShading(
        l,
		lightStrength,
		lightVec,
        normal,
		toEye,
        shadow,
		mat,
		ambientColor,
		diffuseColor,
		specularColor
	);
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

float shadowCalculation(vec4 fragPosLightSpace, NormalShadowMap shadowMap,vec3 normal, vec3 lightDir) {
    float bias = 0.001;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // z값을 조금 높여 색을 가져온다. 큐브와 바닥이 바짝 붙어있으면 그림자가 생기지 않는 이슈가 있다.
    projCoords.z = projCoords.z + 0.01;

    float closestDepth = texture(shadowMap.depthMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap.depthMap, 0);
    for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(shadowMap.depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

    return shadow / 9.0;
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
