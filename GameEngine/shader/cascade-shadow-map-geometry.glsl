#version 450 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in int layerIndex[];

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        gl_Layer = layerIndex[i];  // �� �ν��Ͻ��� �ٸ� ���̾ �������ǵ��� ����
        EmitVertex();
    }
    EndPrimitive();
}