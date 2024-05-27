#include "GBuffer.h"
#include "../Constants.h"
#include <iostream>
#include "../Util/Shader.h"
#include "../Editor/EditorSharedValue.h"

GBuffer::GBuffer() {
	glGenFramebuffers(1, &gBufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

	createGBufferTexture(positionMetallicTexture);
	createGBufferTexture(albedoRoughnessTexture);
	createGBufferTexture(normalTexture);
	createGBufferTexture(godRayTexture);

	unsigned int* attachments = new unsigned int[currentTextureIndex];

	for(int i = 0; i < currentTextureIndex; i++) {
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glDrawBuffers(currentTextureIndex, attachments);

	// rbo »ý¼º
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	delete[] attachments;
}

void GBuffer::use() {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(programName);
	glUseProgram(program);
	
	glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void GBuffer::unuse() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::UpdateImGui() {
	EditorSharedValue::albedoRoughnessTexture = albedoRoughnessTexture;
	EditorSharedValue::positionMetallicTexture = positionMetallicTexture;
	EditorSharedValue::normalTexture = normalTexture;
	EditorSharedValue::godRayTexture = godRayTexture;
}

void GBuffer::createGBufferTexture(unsigned int& texture) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0,
		GL_RGBA,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + currentTextureIndex, GL_TEXTURE_2D, texture, 0);
	currentTextureIndex++;
}

GBuffer::~GBuffer() {
	glDeleteFramebuffers(1, &gBufferFBO);
	glDeleteTextures(1, &positionMetallicTexture);
	glDeleteTextures(1, &albedoRoughnessTexture);
	glDeleteTextures(1, &normalTexture);
	glDeleteTextures(1, &godRayTexture);
	glDeleteRenderbuffers(1, &rbo);
}