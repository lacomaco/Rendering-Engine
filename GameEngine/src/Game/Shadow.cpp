#include "Shadow.h"
#include <glew.h>
#include "../Constants.h"

Shadow::Shadow() {
	glGenFramebuffers(1, &depthMapFrameBuffer);

	std::cout << "shadow 생성 " << std::endl;

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		SHADOW_WIDTH,
		SHADOW_HEIGHT,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);



	glBindFramebuffer(
		GL_FRAMEBUFFER,
		depthMapFrameBuffer
	);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		depthMap,
		0
	);

	// 색상버퍼 쓰기 비활성화
	glDrawBuffer(GL_NONE);
	// 깊이버퍼 읽기 비활성화.
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 디버깅용도 vao,vbo 생성
	quadVertices = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		quadVertices.size() * sizeof(float),
		&quadVertices[0],
		GL_STATIC_DRAW
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		2,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		(void*)0
	);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		(void*)(2 * sizeof(float))
	);
}

void Shadow::WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, glm::mat4 lightSpaceMatrix) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram("shadow");

	glUseProgram(program);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	shader->setMat4("shadow", "lightSpaceMatrix", lightSpaceMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		meshRenderer->Draw("shadow");
		glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::showDepthMap() {
	auto shader = Shader::getInstance()->getShaderProgram("shadow-debug");
	glUseProgram(shader);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // 컬러버퍼
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
