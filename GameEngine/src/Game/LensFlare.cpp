#include "LensFlare.h"
#include "../Constants.h"

LensFlare::LensFlare() {
	auto imguiController = ImguiController::getInstance();

	glGenFramebuffers(1, &lensFlareFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lensFlareFBO);

	auto vaoResult = CreateSimpleQuad();
	vao = vaoResult.VAO;
	vbo = vaoResult.VBO;
	
	glGenTextures(1, &downSampledTexture);
	glBindTexture(GL_TEXTURE_2D, downSampledTexture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		WINDOW_WIDTH / 2.0,
		WINDOW_HEIGHT / 2.0,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &lensFlareTexture);
	glBindTexture(GL_TEXTURE_2D, lensFlareTexture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	glFramebufferTexture2D(	
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		lensFlareTexture,
		0
	);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	imguiController->lensFlareTexture = downSampledTexture;
}

void LensFlare::Draw(unsigned int sceneTexture) {
	const char* programName = "lensFlare";

	auto shader = Shader::getInstance();
	glUseProgram(shader->getShaderProgram(programName));


	glBindFramebuffer(GL_FRAMEBUFFER, lensFlareFBO);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		downSampledTexture,
		0
	);

	glViewport(0, 0, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	shader->setInt(programName, "sceneTexture", 0);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		lensFlareTexture,
		0
	);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
