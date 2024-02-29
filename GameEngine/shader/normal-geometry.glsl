#version 410 core
#include common.glsl

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.02;

void GenerateLine(int index)
{
    vec4 pointPosition = model * gl_in[index].gl_Position;
    vec4 pointNormal = normalize(
        (invTranspose * vec4(gs_in[index].normal, 0.0))
    );
    // 원본 포지션
    gl_Position = projection * view * pointPosition;
    EmitVertex();

    gl_Position = projection * view * (pointPosition + pointNormal * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
} 