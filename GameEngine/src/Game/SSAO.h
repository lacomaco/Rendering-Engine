#pragma once
#include <glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <random>
#include <iostream>
#include "Camera.h"
#include <memory>
#include "../Util/GLHelper.h"

class SSAO
{
private:
	
	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;

	void GenerateSampleKernel();

	unsigned int noiseTexture;
	unsigned int ssaoFBO;
	unsigned int vao;
	unsigned int vbo;

	unsigned int ssaoBlurFBO;

public:
	SSAO();

	unsigned int ssaoColorBuffer;
	unsigned int ssaoColorBufferBlur;

	float radius = 0.278;
	float bias = -0.018f;


	void DrawSSAO(
		unsigned int position,
		unsigned int normal,
		std::shared_ptr<Camera> camera
	);

	void UpdateImGui();
};

