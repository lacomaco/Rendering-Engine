#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "BloomFBO.h"

class Bloom
{
private:
	glm::ivec2 viewPortSize;
	glm::vec2 viewPortSizeFloat;

	void DownSample(unsigned int textureId);
	void UpSample();
public:
	Bloom();
	~Bloom() = default;
	void Draw(unsigned int textureId);
	std::shared_ptr<BloomFBO> fbo;
	unsigned int GetTexture();

	float filterRadius = 0.005;
	int numMips = 5;

};

