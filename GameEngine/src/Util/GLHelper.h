#pragma once
#include <vector>
#include <glm/glm.hpp>

extern std::vector<float> quadVertices;

// PostProcessing에 사용되는 Quad 생성 결과.
struct SimpleQuad {
	unsigned int VAO;
	unsigned int VBO;
	std::vector<float> quadVertices = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
};

SimpleQuad CreateSimpleQuad();

// 화면에 꽉차는 텍스처 생성
unsigned int CreateSimpleTexture();

glm::vec2 WorldToScreen(glm::vec3 worldPos, glm::mat4 view, glm::mat4 position);
