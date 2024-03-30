#include "LensFlare.h"
#include "../Constants.h"
#include "../Util/stb_image.h"

LensFlare::LensFlare() {
	bloom = std::make_shared<Bloom>();

	auto imguiController = ImguiController::getInstance();

	glGenFramebuffers(1, &lensFlareFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lensFlareFBO);

	auto vaoResult = CreateSimpleQuad();
	vao = vaoResult.VAO;
	vbo = vaoResult.VBO;

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

	glGenTextures(1, &dirtLensTexture);
	glBindTexture(GL_TEXTURE_2D, dirtLensTexture);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("./assets/lens-dirt.jpeg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			data
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
	else {
		std::cerr << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	imguiController->lensFlareTexture = lensFlareTexture;
	imguiController->uGhosts = uNumGhosts;
	imguiController->uGhostDispersal = uGhostDispersal;
}

void LensFlare::Draw(unsigned int sceneTexture,unsigned int godRayTexture) {
	UpdateImGui();
	const char* programName = "lensFlare";
	const char* dirtProgramName = "lens-dirt";

	bloom->Draw(godRayTexture);

	auto shader = Shader::getInstance();
	glUseProgram(shader->getShaderProgram(programName));


	glBindFramebuffer(GL_FRAMEBUFFER, lensFlareFBO);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	shader->setInt(programName, "sceneTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloom->fbo->textures[0]->texture);
	shader->setInt(programName, "godRayTexture", 1);

	shader->setFloat(programName, "uGhostDispersal", uGhostDispersal);
	shader->setInt(programName, "uGhostCount", uNumGhosts);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	bloom->Draw(lensFlareTexture);
	glUseProgram(
		shader->getShaderProgram(dirtProgramName)
	);

	glBindFramebuffer(GL_FRAMEBUFFER, lensFlareFBO);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bloom->fbo->textures[0]->texture);
	shader->setInt(dirtProgramName, "bloomTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dirtLensTexture);
	shader->setInt(dirtProgramName, "dirtLensTexture", 1);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LensFlare::UpdateImGui() {
	auto imguiController = ImguiController::getInstance();

	uGhostDispersal = imguiController->uGhostDispersal;
	uNumGhosts = imguiController->uGhosts;
}
