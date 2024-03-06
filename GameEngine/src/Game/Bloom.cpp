#include "Bloom.h"
#include "../Constants.h"
#include <glew.h>
#include "../Util/Shader.h"

void Bloom::Draw(unsigned int textureId) {
	fbo->use();

	DownSample(textureId);
	UpSample();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Bloom::DownSample(unsigned int textureId) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram("down-sample");
	glUseProgram(program);

	shader->setVec2("down-sample","resolution",viewPortSizeFloat);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	auto textureLengths = fbo->GetSize();

	for (int i = 0; i < textureLengths; i++) {
		auto& texture = fbo->textures[i];
		glViewport(0,0,texture->size.x,texture->size.y);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			texture->texture,
			0
		);

		glBindVertexArray(fbo->vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		shader->setVec2("down-sample", "resolution", texture->size);
		glBindTexture(GL_TEXTURE_2D, texture->texture);
	}
}

void Bloom::UpSample() {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram("up-sample");
	glUseProgram(program);

	shader->setFloat("up-sample", "filterRadius", filterRadius);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	auto chainSize = fbo->GetSize();

	for (int i = chainSize - 1; i > 0; i--) {
		auto& texture = fbo->textures[i];
		auto& prevTexture = fbo->textures[i - 1];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->texture);
		glViewport(0, 0, prevTexture->size.x, prevTexture->size.y);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			prevTexture->texture,
			0
		);

		glBindVertexArray(fbo->vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	glDisable(GL_BLEND);

}

unsigned int Bloom::GetTexture() {
	return fbo->GetTexture();
}

Bloom::Bloom() {
	viewPortSize = glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	viewPortSizeFloat = glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT);

	fbo = std::make_shared<BloomFBO>(WINDOW_WIDTH, WINDOW_HEIGHT, numMips);
}

