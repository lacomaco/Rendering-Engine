#version 460 core
#include common.glsl
layout (location = 0) out vec4 positionMetallic;
layout (location = 1) out vec4 albedoRoughness;
layout (location = 2) out vec4 normal;
layout (location = 3) out vec4 godRays;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;
in vec3 normalWorld;

uniform bool isGodRay;

void main() {

    vec3 _normal = normalWorld;

    if(use_normal0){
        _normal = getNormal(normal0, TexCoords, TBN);
    }

    normal = vec4(_normal,0.0);

	float roughness = use_roughness0 ? texture(roughness0, TexCoords).g : 0.0;
	float metallic = use_metallic0 ? texture(metallic0, TexCoords).b : 0.0;
    vec3 albedo = texture(albedo0, TexCoords).rgb;

    positionMetallic = vec4(FragPos, metallic);
	albedoRoughness = vec4(albedo, roughness);

    if(isGodRay){
        godRays = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    } else {
        godRays = vec4(0.0,0.0,0.0,0.0);
    }

}
