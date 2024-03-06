#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../Util/GLHelper.h"
#include "BloomTexture.h"
#include <memory>

class BloomFBO
{
private:
	std::vector<float> quad = quadVertices;
	unsigned int vbo;

public:
	BloomFBO(int width, int height, int bloomChains);
	~BloomFBO();
	unsigned int fbo;
	void use();

	unsigned int vao;

	std::vector<std::shared_ptr<BloomTexture>> textures;


	unsigned int GetTexture() {
		return textures[0]->texture;
	}

	int GetSize() {
		return textures.size();
	}
};

