#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include "Bloom.h"
#include "GodRays.h"
#include "GBuffer.h"
#include "MeshRenderer.h"
#include "SSAO.h"
#include "LensFlare.h"

class GraphicsPipeLine
{
private:
	void CreateVAO();
	void CreateLigtingFramebuffer();
	const char* programName = "gBuffer";

public:
	GraphicsPipeLine();
	void PutExposure(const char* programName);

	void DrawGBuffer(
		shared_ptr<MeshRenderer> mesh
	);

	void DrawGBuffer(shared_ptr<MeshRenderer> mesh,
		glm::vec3 lightPos);

	void DrawSSAO();

	unsigned int lightingFrameBuffer;
	unsigned int lightingTexture; // 컬러버퍼 텍스처, MSAA

	unsigned int rbo;
	unsigned int vao;
	unsigned int vbo;

	void Draw(const char* programName);

	std::shared_ptr<Bloom> bloom;
	std::shared_ptr<GBuffer> gBuffer;
	std::shared_ptr<GodRays> godRays;
	std::shared_ptr<SSAO> ssao;
	std::shared_ptr<LensFlare> lensFlare;

	float exposure = 1.0f;

	float bloomThreshold = 0.04f;

	bool useSSAO = true;

	void UpdateImGui();
	void DefferedLighting();
};

