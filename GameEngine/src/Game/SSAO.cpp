#include "SSAO.h"
#include "../Util/CommonMath.h"
#include "../Constants.h"
#include "../Util/Shader.h"
#include "../Editor/EditorSharedValue.h"

SSAO::SSAO() {
	EditorSharedValue::radius = radius;
	EditorSharedValue::bias = bias;

	GenerateSampleKernel();

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// FBO ����
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	// SSAO �÷� ���� ����
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0,
		GL_RED,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		ssaoColorBuffer,
		0
	);

	// vao, vbo ����
	auto result = CreateSimpleQuad();
	vao = result.VAO;
	vbo = result.VBO;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ������ ���� FBO
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glBindVertexArray(vao);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0,
		GL_RED,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		ssaoColorBufferBlur,
		0
	);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::DrawSSAO(unsigned int position, unsigned int normal,std::shared_ptr<Camera> camera) {
	const char* programName = "SSAO";
	const char* blurProgramName = "SSAOBlur";
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(programName);
	glUseProgram(program);

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glBindVertexArray(vao);
	glViewport(0,0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, position);
	shader->setInt(programName, "positionTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	shader->setInt(programName, "normalTexture", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	shader->setInt(programName, "noiseTexture", 2);

	// Ŀ�� ������ ����
	for (unsigned int i = 0; i < ssaoKernel.size(); i++) {
		auto& sample = ssaoKernel[i];
		shader->setVec3(programName, ("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
	}

	shader->setMat4(programName, "projection", camera->projection);
	shader->setMat4(programName, "view", camera->view);

	shader->setFloat(programName, "width", (float)WINDOW_WIDTH);
	shader->setFloat(programName, "height", (float)WINDOW_HEIGHT);

	shader->setFloat(programName, "radius", radius);
	shader->setFloat(programName, "bias", bias);

	glDisable(GL_BLEND);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_BLEND);

	// Blurring
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	program = shader->getShaderProgram(blurProgramName);
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	shader->setInt(blurProgramName, "ssaoTexture", 0);

	glDisable(GL_BLEND);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	UpdateImGui();
}

void SSAO::GenerateSampleKernel() {
	if (ssaoKernel.size() != 0) {
		ssaoKernel.clear();
	}

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < 64; i++) {
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0, // -1 ~ 1 ����ȭ.
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) // Tangent Space�̱⿡ z���� 0~1
		);

		sample = glm::normalize(sample); // ���� ũ�⸦ 1�� ����� �ݱ��� ���� ����
		sample *= randomFloats(generator);

		/*
		* scale * scale�� ���ִ� ����
		* ���� ������ �ݱ������� ���� ������µ� �̰��� �� �� ������ ������ �ϱ� ���ؼ� �߰���.
		* scale * scale�� ���� �Լ������� �׷����� ��������µ�
		* 
		* i�� ���� ������ ���� �����ϰ� ���ø���.
		*/
		float scale = (float)i / 64.0;
		scale = CommonMath::Lerp(0.1f, 1.0f, scale * scale);

		sample *= scale;

		ssaoKernel.push_back(sample);
	}

	// �ݱ��� Ŀ�� �����ϰ� ȸ����ų ������ ����
	if (ssaoNoise.size() != 0) {
		ssaoNoise.clear();
	}

	for (int i = 0; i < 16; i++) {
		/*
		* ������� xy ��鿡�� ������.
		* 
		* 
		*/
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f
		);

		ssaoNoise.push_back(
			glm::normalize(noise)
		);
	}
}

void SSAO::UpdateImGui() {
	EditorSharedValue::ssaoTexture = ssaoColorBuffer;
	EditorSharedValue::ssaoBlurTexture = ssaoColorBufferBlur;

	radius = EditorSharedValue::radius;
	bias = EditorSharedValue::bias;
}
