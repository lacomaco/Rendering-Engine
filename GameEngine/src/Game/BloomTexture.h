#pragma once
#include <glm/glm.hpp>
class BloomTexture
{
public:
	glm::vec2 size;
	glm::ivec2 intSize;

	unsigned int texture;

	BloomTexture(glm::vec2 size, glm::vec2 intSize);
	~BloomTexture();
};

