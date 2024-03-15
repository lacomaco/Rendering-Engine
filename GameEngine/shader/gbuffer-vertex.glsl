#version 460 core
#include common.glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangentModel;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN;
out vec3 normalWorld;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoord;
    FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 normal = normalize(vec3(invTranspose * vec4(aNormal, 0.0)));
    
    normalWorld = normal;

    vec3 T = aTangentModel;
    if(dot(cross(normal, T), aBitangent) < 0.0){
        T = -T;
    }

    T = normalize(vec3(invTranspose * vec4(T, 0.0)));
    vec3 B = normalize(vec3(invTranspose * vec4(aBitangent, 0.0)));

    TBN = mat3(T, B, normal);

}