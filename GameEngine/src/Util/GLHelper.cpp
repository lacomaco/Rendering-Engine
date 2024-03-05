#include "GLHelper.h"
#include <glew.h>
#include <iostream>
#include "../Constants.h"

SimpleQuad CreateSimpleQuad() {
	unsigned int vao, vbo;
	SimpleQuad result = { 0, 0 };

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		result.quadVertices.size() * sizeof(float),
		result.quadVertices.data(),
		GL_STATIC_DRAW
	);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	result.VAO = vao;
	result.VBO = vbo;

	return result;
}

unsigned int CreateSimpleTexture() {
	unsigned int texture = 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}
