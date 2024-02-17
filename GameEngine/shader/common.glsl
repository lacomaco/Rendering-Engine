uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 invTranspose;


uniform sampler2D albedo0;
uniform sampler2D specular0;

struct Material {
	vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

struct Light {
	vec3 direction; // Directional Light 에서만 사용함.
    vec3 position;
    int lightType; // 0: directional, 1: point, 2: spot

    // 빛감쇠
    float constant;
    float linear;
    float quadratic;

    // phong shading 전용 PBR에서 지울거임.
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

uniform vec3 cameraPos;

float calcAttenuation(float distance) {
	return 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
}

vec3 phongShading(
    float lightStrength,
    vec3 lightDirection,
    vec3 normal,
    vec3 toEye,
    Material mat,
    vec3 ambientColor,
    vec3 diffuseColor,
    vec3 specularColor
    ) {
	vec3 ambient = light.ambient * ambientColor;
    vec3 diffuse = lightStrength * light.diffuse * diffuseColor;

    vec3 reflectDir = reflect(lightDirection, normal);
    float spec = pow(max(dot(toEye, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * specularColor * light.specular;

	return ambient + diffuse + specular;
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
	vec3 lightVec = normalize(l.position - posWorld);
    float lightStrength = max(dot(normal, lightVec), 0.0);
    float distance = length(l.position - posWorld);
    float attenuation = calcAttenuation(distance);

    ambientColor *= attenuation;
    diffuseColor *= attenuation;
    specularColor *= attenuation;


    return phongShading(
        lightStrength,
        lightVec,normal,
        toEye,
        mat,
        ambientColor,
        diffuseColor,
        specularColor
    );
}

vec3 spotLight() {
    return vec3(0.0);
}
