#define MAX_LIGHTS 10

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 invTranspose;


uniform sampler2D albedo0;
uniform sampler2D specular0;


uniform samplerCube skyBox;
uniform samplerCube radianceMap;
uniform samplerCube irradianceMap;

struct Material {
    // phong shading 전용.
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

uniform Material material;

uniform int activeLight;

struct Light {
	vec3 direction; // Directional Light 에서만 사용함.
    vec3 position;
    int lightType; // 0: directional, 1: point, 2: spot

    // 빛감쇠
    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float cutOuter;
};

uniform Light lights[MAX_LIGHTS];

uniform vec3 cameraPos;

float calcAttenuation(float distance,Light l) {
	return 1.0 / (l.constant + l.linear * distance + l.quadratic * (distance * distance));
}

vec3 phongShading(
    Light l,
    float lightStrength,
    vec3 toLightDirection, // 지표면 <- 빛 방향이다.
    vec3 normal,
    vec3 toEye,
    Material mat,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor
    ) {
	vec3 ambient = mat.ambient * ambientColor;

    vec3 diffuse = lightStrength * mat.diffuse * diffuseColor;

    vec3 reflectDir = reflect(-toLightDirection, normal);
    float spec = pow(max(dot(toEye, reflectDir), 0.0), mat.shininess);

	vec3 specular = specularColor * mat.specular;

	return ambient + diffuse;
}

vec3 directionalLight(
    Light l, 
    Material mat,
    vec3 posWorld,
    vec3 normal,
    vec3 toEye,
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
    float intensity = clamp((theta - l.cutOuter) / epsilon, mat.ambient.r, 1.0);

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
		mat,
		ambientColor,
		diffuseColor,
		specularColor
	);
}
