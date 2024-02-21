#pragma once
#include <iostream>
#include <vector>

class PostProcessingFrameBuffer
{
public:
	PostProcessingFrameBuffer();
	unsigned int framebuffer;
	unsigned int textureColorbuffer; // 컬러버퍼 텍스처
	unsigned int rbo;
	unsigned int vao;
	unsigned int vbo;
	void use();

	std::vector<float> quadVertices;

	void Draw(const char* programName);
};

