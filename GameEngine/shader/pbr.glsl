
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
    vec3 posWorld;
	float metallic;
	float roughness;
	float ao;
};


float calcAttenuation(float distance,Light l) {
	return 1.0 / (l.constant + l.linear * distance + l.quadratic * distance);
}

vec3 SchlickFresnel(vec3 F0, float ndotH){
    return F0 + (1.0 - F0) *  pow(2.0, (-5.55473 * ndotH - 6.98316) * ndotH);
}

vec3 diffuseIBL(vec3 albedo,vec3 normalWorld,vec3 pixelToEye,float metallic) {
    vec3 F0 = mix(Fdielectric, albedo, metallic);
    vec3 F = SchlickFresnel(F0, max(dot(normalWorld,pixelToEye),0.0));
    vec3 kd = mix(1.0 - F, vec3(0.0), metallic);

    vec3 irradiance = textureLod(irradianceMap, normalWorld,0.0).rgb;

    return kd * albedo * irradiance;
}

vec3 specularIBL(vec3 albedo, vec3 normalWorld, vec3 pixelToEye, float metallic,float roughness) {
    vec2 brdf = texture(
        brdfTexture, 
        vec2(dot(pixelToEye,normalWorld),1.0 - roughness)
    ).rg;

    vec3 specularIrradiance = textureLod(radianceMap, reflect(-pixelToEye,normalWorld), 2.0 + roughness * 10.0f).rgb;
    vec3 F0 = mix(Fdielectric, albedo, metallic);

    return (F0 * brdf.x  + brdf.y) * specularIrradiance;
}


// 나중에 GI 구현하게되면 diffuseIBL에 ndotl을 곱하는 코드는 제거해야함.
vec3 ambientIBL(vec3 albedo, vec3 normalW, vec3 pixelToEye, float ao, float metallic,float roughness,float shadow,float ndotl){
    vec3 diffuseIBL = diffuseIBL(albedo,normalW,pixelToEye,metallic);
    vec3 specularIBL = specularIBL(albedo,normalW,pixelToEye,metallic,roughness);

    specularIBL *= shadow;
    diffuseIBL *= ndotl < 0.2 ? 0.2 : ndotl;

    return (diffuseIBL + specularIBL) * ao;
}

float NdfGGX(float ndotH, float roughness) {
    float pi = 3.141592;
    float alpha = pow(roughness, 2);
    float alpha2 = pow(alpha, 2);
    float denom = pow(ndotH,2) * (alpha2 - 1) + 1;

    return alpha2 / pi * pow(denom, 2);
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

    vec3 specularBRDF = (D * G * F) / max((4 * value.ndotl * value.ndotO),Epsilon);

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
	vec3 toLight = normalize(l.position - value.posWorld);
    float lightStrength = max(dot(value.normalWorld, toLight), 0.0);
    float distance = length(l.position - value.posWorld);
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

struct ShadowStruct {
    bool isShadow;
    float shadow;
};

ShadowStruct shadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap,vec3 normal, vec3 lightDir) {
    ShadowStruct result;

    float bias = 0.001;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // 좌표를 조금 높혀서 가져온다. 큐브와 바닥이 바짝 붙어있으면 그림자가 생기지 않는 이슈가 있다.
    projCoords += lightDir * 0.0001;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    result.isShadow = currentDepth - bias > closestDepth;

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    const int halfkernelWidth = 2;

    for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x) {
        for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    result.shadow = shadow / ((halfkernelWidth * 2 + 1) * (halfkernelWidth * 2 + 1));

    return result;
}

ShadowStruct pointShadowCalculation(vec3 posWorld,Light light,samplerCube map){
	ShadowStruct result;
    vec3 posToLight = posWorld - light.position;

    float closestDepth = texture(map, posToLight).r;

    closestDepth *= far;

    float currentDepth = length(posToLight);

    float bias = 0.05;

    // 나중에 시간이 되면 여기에도 PCF, PCSS를 적용하자.
    result.isShadow = currentDepth - bias > closestDepth;
    result.shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return result;
}
