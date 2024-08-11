
uniform float shCoeffs[27];

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
	return 1.0 / (l.constant + l.linear * distance + l.quadratic * distance * distance);
}

vec3 SchlickFresnel(vec3 F0, float ndotH){
    return F0 + (1.0 - F0) *  pow(clamp(1.0 - max(ndotH,0.0), 0.0, 1.0), 5.0);
}

vec3 diffuseIBL(vec3 albedo,vec3 normalWorld,vec3 pixelToEye,float metallic) {
    vec3 F0 = mix(Fdielectric, albedo, metallic);
    vec3 F = SchlickFresnel(F0, max(dot(normalWorld,pixelToEye),0.0));
    vec3 kd = mix(1.0 - F, vec3(0.0), metallic);

    // vec3 irradiance = textureLod(irradianceMap, normalWorld,0.0).rgb;

    vec3 irradiance = computeF(normalWorld, shCoeffs);

    return kd * albedo * irradiance;
}

vec3 specularIBL(vec3 albedo, vec3 normalWorld, vec3 pixelToEye, float metallic,float roughness) {
    vec2 brdf = texture(
        brdfTexture, 
        vec2(dot(pixelToEye,normalWorld),roughness)
    ).rg;

    vec3 specularIrradiance = textureLod(preFilterEnvironmentMap, reflect(-pixelToEye,normalWorld), 2.0 + roughness * 10.0f).rgb;
    vec3 F0 = mix(Fdielectric, albedo, metallic);

    return (F0 * brdf.x  + brdf.y) * specularIrradiance;
}


vec3 ambientIBL(vec3 albedo, vec3 normalW, vec3 pixelToEye, float ao, float metallic,float roughness){
    vec3 diffuseIBL = diffuseIBL(albedo,normalW,pixelToEye,metallic) * 0.5;
    vec3 specularIBL = specularIBL(albedo,normalW,pixelToEye,metallic,roughness);

    return (diffuseIBL + specularIBL) * ao;
}

float NdfGGX(float ndotH, float roughness) {
    float pi = 3.141592;
    float alpha = pow(roughness, 2);
    float alpha2 = pow(alpha, 2);
    float denom = pow(ndotH,2) * (alpha2 - 1) + 1;

    return alpha2 / (pi * denom * denom);
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
    vec3 kd = mix(1.0 - F, vec3(0.0), value.metallic); // 에너지 보존법칙. Specular가 늘면 diffuse는 내려가야함.
    vec3 diffuseBRDF = kd * value.albedo;

    float D = NdfGGX(value.ndotH, value.roughness);
    float G = SchlickGGX(value.ndotl, value.ndotO, value.roughness);

    vec3 specularBRDF = (D * G * F) / max((4 * value.ndotl * value.ndotO),Epsilon);

    return (diffuseBRDF + specularBRDF) * lightStrength * (1.0 - shadow);
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

    vec3 lightStrength = vec3(max(dot(value.normalWorld, lightVec), 0.0)) * l.strength;

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

    return PBR(value,vec3(lightStrength),shadow) * attenuation;
}

vec3 spotLight(    
    Light l,
    float shadow,
    PBRValue value
    ) {
    vec3 lightVec = normalize(l.position - value.pixelToEye);

    float theta = dot(lightVec, normalize(-l.direction));
    float epsilon = l.cutOff - l.cutOuter;
    float intensity = clamp((theta - l.cutOuter) / epsilon, 0.1, 5.0);

    float distance = length(l.position - value.pixelToEye);
    float attenuation = calcAttenuation(distance,l);

    vec3 lightStrength = vec3(attenuation * intensity);

    vec3 halfWaydir = normalize(lightVec + value.pixelToEye);
    float ndotH = max(dot(value.normalWorld,halfWaydir),0.0);
    float ndotl = max(dot(value.normalWorld,lightVec),0.0);

    value.ndotH = ndotH;
	value.ndotl = ndotl;
    value.halfWayDir = halfWaydir;

    return PBR(value,lightStrength,shadow);
}

struct ShadowStruct {
    bool isShadow;
    float shadow;
    int layer;
};

const vec2 diskSamples16[16] = vec2[](
    vec2( -0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2( -0.094184101, -0.92938870 ),
    vec2( 0.34495938, 0.29387760 ),
    vec2( -0.91588581, 0.45771432 ),
    vec2( -0.81544232, -0.87912464 ),
    vec2( -0.38277543, 0.27676845 ),
    vec2( 0.97484398, 0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2( -0.26496911, -0.41893023 ),
    vec2( 0.79197514, 0.19090188 ),
    vec2( -0.24188840, 0.99706507 ),
    vec2( -0.81409955, 0.91437590 ),
    vec2( 0.19984126, 0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
);

// NDC to View
float N2V(float ndcDepth, mat4 invProj) {
    vec4 pointNDC = vec4(0.0, 0.0, ndcDepth * 2.0 - 1.0, 1.0); // NDC depth to [-1, 1]
    vec4 pointView = invProj * pointNDC;
    return pointView.z / pointView.w; // Perspective divide to get view space depth
}

// Bilinear 필터링을 적용하는 함수
float bilinearPCF(sampler2DArray shadowMap, vec3 uvIndex, mat4 invProj, float compare) {
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;

    vec2 uv00 = uvIndex.xy + texelSize * vec2(-0.5, -0.5);
    vec2 uv10 = uvIndex.xy + texelSize * vec2(0.5, -0.5);
    vec2 uv01 = uvIndex.xy + texelSize * vec2(-0.5, 0.5);
    vec2 uv11 = uvIndex.xy + texelSize * vec2(0.5, 0.5);

    float depth00 = texture(shadowMap, vec3(uv00, uvIndex.z)).r;
    float depth10 = texture(shadowMap, vec3(uv10, uvIndex.z)).r;
    float depth01 = texture(shadowMap, vec3(uv01, uvIndex.z)).r;
    float depth11 = texture(shadowMap, vec3(uv11, uvIndex.z)).r;

    // NDC에서 View z좌표로 변환
    depth00 = N2V(depth00, invProj);
    depth10 = N2V(depth10, invProj);
    depth01 = N2V(depth01, invProj);
    depth11 = N2V(depth11, invProj);

    float factorX = fract(uvIndex.x * textureSize(shadowMap, 0).x);
    float factorY = fract(uvIndex.y * textureSize(shadowMap, 0).y);

    float depth0 = mix(depth00, depth10, factorX);
    float depth1 = mix(depth01, depth11, factorX);
    float depth = mix(depth0, depth1, factorY);

    return compare < depth ? 1.0 : 0.0;
}

float PCF(float currentDepth,int index,vec3 projCoords){
    float shadow = 0.0;

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMaps, 0));

    for(int x = 0; x < 16; ++x) {
        vec2 sampleUV = projCoords.xy + diskSamples16[x] * texelSize;
        shadow += bilinearPCF(shadowMaps, vec3(sampleUV, index), invProjMatrices[index], currentDepth);
	}

    return shadow / 16.0f;
}


ShadowStruct cascadeShadowCalculation(
    vec3 fragPosWorldSpace,
    int shadowIndex,
    Light light
){
    ShadowStruct result;

    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for(int i = 0; i < directionalCascadeLevel; i++){
        if(depthValue < cascadePlaneDistances[i]){
            layer = i;
            break;
        }
    }

    if(layer == -1){
        layer = directionalCascadeLevel - 1;
    }

    result.layer = layer;

    vec4 fragInLightProjection = lightSpaceMatrices[shadowIndex + layer] * vec4(fragPosWorldSpace,1.0);
    vec3 projCoords = fragInLightProjection.xyz / fragInLightProjection.w;
    projCoords = projCoords * 0.5 + 0.5;

    vec4 fragInLightView = lightViewMatrices[shadowIndex + layer] * vec4(fragPosWorldSpace, 1.0);
    float currentDepth = fragInLightView.z;

    result.shadow = PCF(currentDepth,shadowIndex + layer, projCoords);

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
