#version 410 core
#include common.glsl

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
