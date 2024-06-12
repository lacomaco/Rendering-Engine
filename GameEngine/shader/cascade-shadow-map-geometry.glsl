#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in int layerIndex[];

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        gl_Layer = layerIndex[i];  // 각 인스턴스가 다른 레이어에 렌더링되도록 설정
        EmitVertex();
    }
    EndPrimitive();
}