#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include "Bloom.h"
#include "GodRays.h"

class GraphicsPipeLine
{
private:
	void CreateVAO();
	void CreateMSAAFrameBuffer();
	void CreateIntermediateFrameBuffer();

public:
	GraphicsPipeLine();
	void PutExposure(const char* programName);

	unsigned int msaaFrameBuffer;
	unsigned int msaaTexture; // 컬러버퍼 텍스처, MSAA

	unsigned int intermediateFrameBuffer;
	unsigned int screenTexture; // 컬러버퍼 텍스처, MSAA 해제

	unsigned int rbo;
	unsigned int vao;
	unsigned int vbo;
	void use();

	void Draw(const char* programName);

	std::shared_ptr<Bloom> bloom;
	std::shared_ptr<GodRays> godRays;

	float exposure = 1.0f;

	float bloomThreshold = 0.04f;
};

