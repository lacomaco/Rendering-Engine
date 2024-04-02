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
    vec3 normal = normalize(invTranspose * aNormal);
    
    normalWorld = normal;

    vec3 T = aTangentModel;
    if(dot(cross(normal, T), aBitangent) < 0.0){
        T = -T;
    }

    T = normalize(invTranspose * T);
    vec3 B = normalize(invTranspose * aBitangent);

    TBN = mat3(T, B, normal);

}