#include "Bloom.h"
#include "../Util/GLHelper.h"
#include <glew.h>
#include "../Constants.h"
#include <iostream>
#include "../Util/Shader.h"

Bloom::Bloom() {

	auto quad = CreateSimpleQuad();

	vao = quad.VAO;
	vbo = quad.VBO;

	CreateSceneTexture();
	/*
	* 하나의 프레임버퍼에서 읽고 쓰기를 동시에 할 수 없기 때문에 2개의 버퍼를 만든후
	* 교체하면서 블룸효과를 넣는다.
	*/
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);

	for (int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			pingpongColorbuffers[i],
			0
		);

		// opengl 에러 체크
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::CreateSceneTexture() {
	glGenTextures(1, &sceneTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, sceneTexture);
	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE,
		4,
		GL_RGBA32F,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		GL_TRUE
	);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

void Bloom::CopySceneTexture(unsigned int msaaFB) {
	
	// 프레임버퍼 청소
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFB);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pingpongFBO[1]);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glReadBuffer(GL_COLOR_ATTACHMENT1);

	glBlitFramebuffer(
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);

	// read,draw 복구
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::Draw() {
	bool horizontal = true;
	auto shader = Shader::getInstance();
	auto progrma = shader->getShaderProgram("bloom");
	glUseProgram(progrma);

	for (int i = 0; i < bloomCount; i++) {
		int index = horizontal ? 0 : 1;
		shader->setBool("bloom", "horizontal", horizontal);
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[index]);
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(
			GL_TEXTURE_2D,
			pingpongColorbuffers[(index+1)%2]
		);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		horizontal = !horizontal;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Bloom::TextureId() {
	if(bloomCount % 2 == 0)
		return pingpongColorbuffers[0];
	else
		return pingpongColorbuffers[1];
}

void Bloom::Merge() {

}
