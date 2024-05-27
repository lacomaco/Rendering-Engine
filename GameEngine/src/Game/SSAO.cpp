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

	// FBO 생성
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	// SSAO 컬러 버퍼 생성
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

	// vao, vbo 생성
	auto result = CreateSimpleQuad();
	vao = result.VAO;
	vbo = result.VBO;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 블러링을 위한 FBO
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

	// 커널 데이터 전송
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
			randomFloats(generator) * 2.0 - 1.0, // -1 ~ 1 정규화.
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) // Tangent Space이기에 z값이 0~1
		);

		sample = glm::normalize(sample); // 벡터 크기를 1로 만들어 반구형 영역 생성
		sample *= randomFloats(generator);

		/*
		* scale * scale을 해주는 이유
		* 위에 랜덤한 반구형태의 구를 만들었는데 이것을 좀 더 원점에 가깝게 하기 위해서 추가됨.
		* scale * scale인 지수 함수식으로 그래프가 만들어지는데
		* 
		* i가 작은 영역이 좀더 조밀하게 샘플링됨.
		*/
		float scale = (float)i / 64.0;
		scale = CommonMath::Lerp(0.1f, 1.0f, scale * scale);

		sample *= scale;

		ssaoKernel.push_back(sample);
	}

	// 반구형 커널 랜덤하게 회전시킬 노이즈 생성
	if (ssaoNoise.size() != 0) {
		ssaoNoise.clear();
	}

	for (int i = 0; i < 16; i++) {
		/*
		* 노이즈는 xy 평면에만 존재함.
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
