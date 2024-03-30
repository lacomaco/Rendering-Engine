#include "BloomFBO.h"
#include <glew.h>
#include <glm/glm.hpp>
#include <iostream>

BloomFBO::BloomFBO(int width, int height, int bloomChains) {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glm::vec2 mipSize = glm::vec2((float)width, (float)height);
	glm::ivec2 minpIntSize = glm::ivec2(width, height);

	for (int i = 0; i < bloomChains; i++) {
		mipSize /= 2.0f;
		minpIntSize /= 2;
		auto texture = std::make_shared<BloomTexture>(mipSize, minpIntSize);
		textures.push_back(texture);
	}

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		textures[0]->texture,
		0
	);

	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Intermediate Framebuffer is not complete!" << std::endl;
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		quad.size() * sizeof(float),
		quad.data(),
		GL_STATIC_DRAW
	);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

BloomFBO::~BloomFBO() {
	glDeleteFramebuffers(1, &fbo);

	for (auto& texture : textures) {
		texture.reset();
	}
}

void BloomFBO::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
