
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


vec3 ambientIBL(vec3 albedo, vec3 normalW, vec3 pixelToEye, float ao, float metallic,float roughness){
    vec3 diffuseIBL = diffuseIBL(albedo,normalW,pixelToEye,metallic);
    vec3 specularIBL = specularIBL(albedo,normalW,pixelToEye,metallic,roughness);

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

    return PBR(value,vec3(lightStrength),shadow) * attenuation;
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

const vec2 diskSamples64[64] = vec2[](
    vec2(0.0, 0.0), vec2(-0.12499844227275288, 0.000624042775189866), vec2(0.1297518688031755, -0.12006020382326336),
    vec2(-0.017851253586055427, 0.21576916541852392), vec2(-0.1530983013115895, -0.19763833164521946), vec2(0.27547541035593626, 0.0473106572479027),
    vec2(-0.257522587854559, 0.16562643733622642), vec2(0.0842605283808073, -0.3198048832600703), vec2(0.1645196099088727, 0.3129429627830483),
    vec2(-0.3528833088400373, -0.12687935349026194), vec2(0.36462214742013344, -0.1526456341030772), vec2(-0.17384046457324884, 0.37637015407303087),
    vec2(-0.1316547617859344, -0.4125130588224921), vec2(0.3910687393754993, 0.2240317858770442), vec2(-0.45629121277761536, 0.10270505898899496),
    vec2(0.27645268679640483, -0.3974278701387824), vec2(0.06673001731984558, 0.49552709793561556), vec2(-0.39574431915605623, -0.33016879600548193),
    vec2(0.5297612167716342, -0.024557141621887494), vec2(-0.3842909284448636, 0.3862583103507092), vec2(0.0230336562454131, -0.5585422550532486),
    vec2(0.36920334463249477, 0.43796562686149154), vec2(-0.5814490172413539, -0.07527974727019048), vec2(0.4903718680780365, -0.3448339179919178),
    vec2(-0.13142003698572613, 0.5981043168868373), vec2(-0.31344141845114937, -0.540721256470773), vec2(0.608184438565748, 0.19068741092811003),
    vec2(-0.5882602609696388, 0.27536315179038107), vec2(0.25230610046544444, -0.6114259003901626), vec2(0.23098706800827415, 0.6322736546883326),
    vec2(-0.6076303951666067, -0.31549215975943595), vec2(0.6720886334230931, -0.1807536135834609), vec2(-0.37945598830371974, 0.5966683776943834),
    vec2(-0.1251555455510758, -0.7070792667147104), vec2(0.5784815570900413, 0.44340623372555477), vec2(-0.7366710399837763, 0.0647362251696953),
    vec2(0.50655463562529, -0.553084443034271), vec2(8.672987356252326e-05, 0.760345311340794), vec2(-0.5205650355786364, -0.5681215043747359),
    vec2(0.7776435491294021, 0.06815798190547596), vec2(-0.6273416101921778, 0.48108471615868836), vec2(0.1393236805531513, -0.7881712453757264),
    vec2(0.4348773806743975, 0.6834703093608201), vec2(-0.7916014213464706, -0.21270211499241704), vec2(0.7357897682897174, -0.38224784745000717),
    vec2(-0.2875567908732709, 0.7876776574352392), vec2(-0.3235695699691864, -0.7836151691933712), vec2(0.7762165924462436, 0.3631291803355136),
    vec2(-0.8263007976064866, 0.2592816844184794), vec2(0.4386452756167397, -0.7571098481588484), vec2(0.18988542402304126, 0.8632459242554175),
    vec2(-0.7303253445407815, -0.5133224046555819), vec2(0.8939004035324556, -0.11593993515830946), vec2(-0.5863762307291154, 0.6959079795748251),
    vec2(-0.03805753378232556, -0.9177699189461416), vec2(0.653979655650389, 0.657027860897389), vec2(-0.9344208130797295, -0.04310155546401203),
    vec2(0.7245109901504777, -0.6047386420191574), vec2(-0.12683493131695708, 0.9434844461875473), vec2(-0.5484582700240663, -0.7880790100251422),
    vec2(0.9446610338564589, 0.2124041692463835), vec2(-0.8470120123194587, 0.48548496473788055), vec2(0.29904134279525085, -0.9377229203230629),
    vec2(0.41623562331748715, 0.9006236205438447)
);

float PCF(float bias, float closestDepth,float currentDepth,sampler2D shadowMap,vec3 projCoords){
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = 0; x < 64; ++x) {
		float pcfDepth = texture(shadowMap, projCoords.xy + diskSamples64[x] * texelSize).r;
		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	}

    return shadow / 64.0f;
}

ShadowStruct shadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap,vec3 normal,vec3 lightDir) {
    ShadowStruct result;

    float bias = 0.005;

    vec3 projCoords = fragPosLightSpace.xyz;
    projCoords = projCoords * 0.5 + 0.5;


    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    result.isShadow = currentDepth - bias > closestDepth;

    result.shadow = PCF(bias,closestDepth,currentDepth,shadowMap,projCoords);

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
