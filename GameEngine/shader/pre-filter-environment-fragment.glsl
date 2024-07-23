#version 460 core
#include Importance-sample.glsl

in vec3 localPos;
out vec4 FragColor;
uniform samplerCube skyMap;
uniform float roughness;

void main() {
    vec3 N = normalize(localPos);
    vec3 R = N;
    vec3 V = R;

    const int SAMPLE_COUNT = 4096;
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0.0);

    for(int i = 0; i < SAMPLE_COUNT; i++){
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V,H) * H - V);

        float NdotL = max(dot(N,L),0.0);
        if(NdotL > 0.0){
            prefilteredColor += texture(skyMap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}
