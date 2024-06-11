#version 460 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        gl_Layer = gl_InstanceID;  // 각 인스턴스가 다른 레이어에 렌더링되도록 설정
        EmitVertex();
    }
    EndPrimitive();
}