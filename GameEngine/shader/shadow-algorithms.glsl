float PCF(vec2 uv,float zReceiver,float filterRadiusUV,float shadowMapIndex){
    float shadow = 0.0;

    for(int x = 0; x < 16; ++x) {
        vec2 offset = diskSamples16[x] * filterRadiusUV;
        shadow += texture(shadowMaps,vec3(uv + offset, shadowMapIndex)).r;
	}

    return shadow / 16.0f;
}

float PenumbraSize(float zReceiver, float zBlocker) {
    return (zReceiver - zBlocker) / zBlocker;
}

float N2V(float ndcDepth,mat4 invProj){
    vec4 pointView = invProj * vec4(0.0, 0.0, ndcDepth, 1.0);
    return pointView.z / pointView.w;
}

void FindBlocker(out float avgBlockerDepth,out float numBlockers,vec2 uv,float zReceiver,int index,float uvSize){
    float searchWidth = uvSize * (zReceiver - near) / zReceiver;

    float blockerSum = 0;
    numBlockers = 0.00001;

    for(int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i) {
        float shadowDepth = 
            texture(shadowMaps,vec3(uv + diskSamples16[i] * searchWidth, index),0).r;

        shadowDepth = N2V(shadowDepth, invLightSpaceMatrices[index]);

        if(shadowDepth < zReceiver){
            blockerSum += shadowDepth;
            numBlockers++;
        }
    }

    avgBlockerDepth = blockerSum / numBlockers;
}

float PCSS(vec2 uv,float zReceiverNdc,int shadowMapIndex){
    float lightSizeUV = LIGHT_WORLD_SIZE / 0.344;

    float zReceiver = N2V(zReceiverNdc, invLightSpaceMatrices[shadowMapIndex]);

    // 1. blocker search
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    FindBlocker(avgBlockerDepth, numBlockers, uv,zReceiver,shadowMapIndex,lightSizeUV);

    if(numBlockers < 1) {
        return 0.0f;
    }

    // 2. calculate penumbraSize
    float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
    float filterRadiusUV = penumbraRatio * lightSizeUV * near / zReceiver;

    return PCF(uv,zReceiver,filterRadiusUV,shadowMapIndex);
}


float cascadeShadowCalculation(
    vec3 fragPosWorldSpace,
    int shadowIndex,
    Light light
){
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

    vec4 fragPosLightSpace = lightSpaceMatrices[shadowIndex + layer] * vec4(fragPosWorldSpace, 1.0);

    vec3 homoFragPosLightSpace = vec3(fragPosLightSpace.xyz) / fragPosLightSpace.w;

    vec3 projCoords = homoFragPosLightSpace;
    projCoords = projCoords * 0.5 + 0.5;

    return PCSS(vec2(projCoords.xy),homoFragPosLightSpace.z,shadowIndex + layer);
}
