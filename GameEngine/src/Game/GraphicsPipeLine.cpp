#include "GraphicsPipeLine.h"
#include <glew.h>
#include "../Constants.h"
#include "../Util/Shader.h"
#include "../Util/GLHelper.h"
#include <memory>
#include "ImguiController.h"

GraphicsPipeLine::GraphicsPipeLine()
{
	auto imgui = ImguiController::getInstance();
	imgui->useSSAO = useSSAO;

	bloom = std::make_shared<Bloom>();
	godRays = std::make_shared<GodRays>();
	gBuffer = std::make_shared<GBuffer>();
	ssao = std::make_shared<SSAO>();
	lensFlare = std::make_shared<LensFlare>();
	CreateVAO();
	CreateMSAAFrameBuffer();
	CreateIntermediateFrameBuffer();
}

void GraphicsPipeLine::Draw(const char* programName)
{
	UpdateImGui();
	auto shader = Shader::getInstance();
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFrameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

	bloom->Draw(screenTexture);
	lensFlare->Draw(screenTexture);


	// Physically Based Bloom 코드는 msaaFrameBuffer에서 resolve한 텍스처를 다시 복사해서 써야함.

	// 기본 컬러버퍼 사용.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(Shader::getInstance()->getShaderProgram(programName));
	PutExposure(programName);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	shader->setInt(programName, "screenTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloom->fbo->textures[0]->texture);
	shader->setInt(programName, "bloomTexture", 1);
	shader->setFloat(programName, "bloomThreshold", bloomThreshold);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, godRays->godRayTexture);
	shader->setInt(programName, "godRayTexture", 2);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ssao->ssaoColorBufferBlur);
	shader->setInt(programName, "ssaoTexture", 3);

	shader->setBool(programName, "useSSAO", useSSAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}

void GraphicsPipeLine::use()
{
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFrameBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void GraphicsPipeLine::PutExposure(const char* programName) {
	auto shader = Shader::getInstance();
	auto progrma = shader->getShaderProgram(programName);
	glUseProgram(progrma);
	shader->setFloat(programName, "exposure", exposure);
}

void GraphicsPipeLine::CreateIntermediateFrameBuffer() {
	glGenFramebuffers(1, &intermediateFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFrameBuffer);

	screenTexture = CreateSimpleTexture();

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		screenTexture,
		0
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Intermediate Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsPipeLine::CreateMSAAFrameBuffer() {
	glGenFramebuffers(1, &msaaFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFrameBuffer);

	// MSAA용 컬러버퍼 텍스처 생성
	glGenTextures(1, &msaaTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaTexture);
	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE,
		4,
		GL_RGBA32F,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		GL_TRUE
	);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// 프레임버퍼 - 텍스처 연결할때 사용하는 함수.
	// target: 연결 대상 프레임버퍼
	// attachment: 텍스처 종료, COLOR_ATTACHMENT0 <- 컬러버퍼
	// texTarget: 연결할 텍스처 종류
	// texTarget : 텍스처 ID
	// level: mip level
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D_MULTISAMPLE,
		msaaTexture,
		0
	);

	// rbo 생성
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(
		GL_RENDERBUFFER,
		4,
		GL_DEPTH24_STENCIL8,
		WINDOW_WIDTH,
		WINDOW_HEIGHT
	);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,
		GL_DEPTH_STENCIL_ATTACHMENT,
		GL_RENDERBUFFER,
		rbo
	);

	// opengl 에러 체크
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsPipeLine::CreateVAO()
{
	SimpleQuad result = CreateSimpleQuad();

	vao = result.VAO;
	vbo = result.VBO;
}

void GraphicsPipeLine::DrawGBuffer(shared_ptr<MeshRenderer> mesh, std::shared_ptr<Camera>camera) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(gBuffer->programName);
	glUseProgram(program);
	gBuffer->use();
	camera->putCameraUniform(gBuffer->programName);
	shader->setBool(gBuffer->programName, "isGodRay", false);
	mesh->Draw(gBuffer->programName);

	gBuffer->UpdateImGui();
}

void GraphicsPipeLine::DrawGBuffer(
	shared_ptr<MeshRenderer> mesh,
	glm::vec3 lightPos,
	std::shared_ptr<Camera> camera
) {
	auto shader = Shader::getInstance();

	DrawGBuffer(mesh, camera);
	auto program = shader->getShaderProgram(gBuffer->programName);
	glUseProgram(program);
	shader->setBool(gBuffer->programName, "isGodRay", true);
	godRays->rayCircle->position = lightPos;
	godRays->rayCircle->Draw(gBuffer->programName);

	gBuffer->UpdateImGui();
}

void GraphicsPipeLine::DrawSSAO(
	std::shared_ptr<Camera> camera
) {
	ssao->DrawSSAO(gBuffer->positionMetallicTexture, gBuffer->normalTexture, camera);
}

void GraphicsPipeLine::UpdateImGui() {
	auto imguiController = ImguiController::getInstance();
	useSSAO = imguiController->useSSAO;
}
