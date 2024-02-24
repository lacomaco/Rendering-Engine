#pragma once
#include <iostream>
#include <vector>

class PostProcessingFrameBuffer
{
private:
	void CreateVAO();
	void CreateMSAAFrameBuffer();
	void CreateIntermediateFrameBuffer();
	void CreateDepthMapFrameBuffer();

	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;

public:
	PostProcessingFrameBuffer();

	unsigned int depthMapFrameBuffer; // 그림자 생성을 위한 프레임버퍼.
	unsigned int depthMap; // 그림자 생성을 위한 텍스처.

	unsigned int msaaFrameBuffer;
	unsigned int msaaTexture; // 컬러버퍼 텍스처, MSAA

	unsigned int intermediateFrameBuffer;
	unsigned int screenTexture; // 컬러버퍼 텍스처, MSAA 해제

	unsigned int rbo;
	unsigned int vao;
	unsigned int vbo;
	void use();

	std::vector<float> quadVertices;

	void Draw(const char* programName);
};

