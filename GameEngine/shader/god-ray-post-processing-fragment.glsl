#version 460 core
// https://medium.com/community-play-3d/god-rays-whats-that-5a67f26aeac2
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D godRayTexture;
uniform vec2 lightPosition;

uniform float decay;
uniform float density;
uniform float weight;
uniform float exposure;

uniform int ghosts;
uniform float ghostDispersal;

void main() {
    int NUM_SAMPLES = 300;
    
    // 현재 픽셀 위치.
    vec2 tc = TexCoords;
    vec2 deltaTexCoord = (tc-lightPosition) / float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;

    vec4 color = texture(godRayTexture, tc);

    for(int i = 0; i < NUM_SAMPLES; i++){
        tc -= deltaTexCoord;
        vec4 textureSample = texture(godRayTexture, tc);
        textureSample *= illuminationDecay * weight;
        color += textureSample;
        illuminationDecay *= decay;
    }

    vec2 reversedTexCoords = vec2(1.0) - TexCoords;
    vec2 texelSize = 1.0 / vec2(textureSize(godRayTexture, 0));
    vec2 ghostVec = (vec2(0.5) - reversedTexCoords) * ghostDispersal;

    vec4 result = vec4(0.0);
    for(int i = 0; i < ghosts; i++){
        vec2 offset = fract(reversedTexCoords + ghostVec * float(i));
        float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
        weight = pow(1.0 - weight, 10.0);
        result += texture(godRayTexture, offset) * weight;
    }

    FragColor = vec4(vec3(color * exposure),1.0) + result;
}
