#include "GLHelper.h"
#include <glew.h>
#include <iostream>
#include "../Constants.h"


std::vector<float> quadVertices = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

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
		GL_RGBA16F,
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

glm::vec2 WorldToScreen(glm::vec3 worldPos, glm::mat4 view, glm::mat4 position) {
	// clipSpace 전환
	glm::vec4 clipSpace = position * view * glm::vec4(worldPos, 1.0f);

	// NDC 전환 쉐이더에서는 GPU에서 자동으로 w 나누기 처리해줌 CPU에서 연산할땐 직접해야함.
	glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;
	// NDC -> 스크린 좌표로 변환
	glm::vec2 screenPos = glm::vec2(
		(ndc.x + 1.0f) * 0.5f * WINDOW_WIDTH,
		(1.0f - ndc.y) * 0.5f * WINDOW_HEIGHT
	);

	return screenPos;
}

