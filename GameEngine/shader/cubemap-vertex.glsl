#version 460 core
#include common.glsl
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * skyBoxView * model* vec4(aPos, 1.0);
}  