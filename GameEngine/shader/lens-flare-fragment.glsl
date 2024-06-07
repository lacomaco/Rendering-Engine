#version 460 core

uniform sampler2D sceneTexture;
uniform sampler2D godRayTexture;

out vec4 FragColor;
in vec2 TexCoords;

vec3 colorBias = vec3(-0.8);
float colorScale = 2.0;

uniform float uGhostDispersal;
uniform int uGhostCount;

//color gradient 생성기

const vec3 centerColor = vec3(0.8, 0.8, 0.8);
const vec3 lightBlue = vec3(0.2, 0.4, 0.6);
const vec3 darkBlue = vec3(0.0, 0.1, 0.2);
const vec3 warmYellow = vec3(0.8, 0.6, 0.2);
const vec3 lightRed = vec3(0.8, 0.2, 0.2);
const vec3 darkRed = vec3(0.4, 0.0, 0.0);

vec3 circularGradient(vec2 uv){
    vec2 center = vec2(0.5);
    float dist = length(uv-center) * 2.0;
    
    const float blueThreshold = 0.2;
    const float yellowThreshold = 0.4;
    const float redThreshold = 0.6;
    const float darkRedThreshold = 0.8;
    
    vec3 color = mix(centerColor, lightBlue, smoothstep(0.0, blueThreshold, dist));
    
    if(dist > blueThreshold){
        color = mix(lightBlue, darkBlue, smoothstep(blueThreshold, yellowThreshold, dist));
    }
    if(dist > yellowThreshold){
        color = mix(darkBlue, warmYellow, smoothstep(yellowThreshold, redThreshold, dist));
    }
    if(dist > redThreshold){
        color = mix(warmYellow, lightRed, smoothstep(redThreshold, darkRedThreshold, dist));
    }
    if(dist > darkRedThreshold){
        color = mix(lightRed, darkRed, smoothstep(darkRedThreshold, 1.0, dist));
    }
    
    // 패턴 추가
    float pattern = sin(dist * 20.0) * 0.5 + 0.5;
    color = mix(color, vec3(0.0), pattern * 0.2);
    
    return color;
}


void main() {
	vec3 color = texture(sceneTexture, TexCoords).rgb;
	color = max(vec3(0.0), color + colorBias) * colorScale;

	// 중심을 기준으로 좌우 반전
	vec2 inversedTexCoord = -TexCoords + vec2(1.0);
	vec2 texelSize = 1.0 / textureSize(godRayTexture, 0);

	vec2 ghostVec = (vec2(0.5) - inversedTexCoord) * uGhostDispersal;

	vec4 result = vec4(0.0);

	for(int i = 0; i < uGhostCount; i++) {
	    vec2 offset = fract(inversedTexCoord + ghostVec * float(i));

		float weight = length(vec2(0.5) - offset) / length(vec2(0.5));
		weight = pow(1.0 - weight, 10.0);

		result += texture(godRayTexture, offset) * weight;
	}

    float haloWidth = 0.4;
    vec2 haloVec = normalize(ghostVec) * haloWidth;
    float weight = length(vec2(0.5) - fract(inversedTexCoord + haloVec)) / length(vec2(0.5));
    weight = pow(1.0 - weight, 5.0);
    result += texture(godRayTexture, fract(inversedTexCoord + haloVec)) * weight;

    result *= vec4(circularGradient(TexCoords),1.0);

    FragColor = result;
}