#include "BloomTexture.h"
#include <glew.h>
#include <iostream>

BloomTexture::BloomTexture(glm::vec2 size, glm::vec2 intSize) {
 	this->size = size;
	this->intSize = intSize;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		size.x,
		size.y,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

BloomTexture::~BloomTexture() {
	glDeleteTextures(1, &texture);
}