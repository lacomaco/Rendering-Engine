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

void main() {
    int NUM_SAMPLES = 100;
    
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

    FragColor = vec4(vec3(color * exposure),1.0);
}
