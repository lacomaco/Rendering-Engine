#include "GraphicsPipeLine.h"
#include <glew.h>
#include "../Constants.h"
#include "../Util/Shader.h"
#include "../Util/GLHelper.h"
#include <memory>
#include "../Editor/EditorSharedValue.h"

GraphicsPipeLine::GraphicsPipeLine()
{
	EditorSharedValue::useSSAO = useSSAO;

	bloom = std::make_shared<Bloom>();
	godRays = std::make_shared<GodRays>();
	gBuffer = std::make_shared<GBuffer>();
	ssao = std::make_shared<SSAO>();
	lensFlare = std::make_shared<LensFlare>();
	CreateVAO();
	CreateLigtingFramebuffer();


	EditorSharedValue::lightingTexture = lightingTexture;
}

void GraphicsPipeLine::Draw(const char* programName)
{
	UpdateImGui();
	auto shader = Shader::getInstance();

	lensFlare->Draw(lightingTexture, gBuffer->godRayTexture);
	bloom->Draw(lightingTexture);

	if (EditorSharedValue::editorMode) {
		glBindFramebuffer(GL_FRAMEBUFFER, EditorSharedValue::EditorMainSceneFrameBuffer);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(Shader::getInstance()->getShaderProgram(programName));
	PutExposure(programName);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightingTexture);
	shader->setInt(programName, "lightingTexture", 0);
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

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lensFlare->lensFlareTexture);
	shader->setInt(programName, "lensFlareTexture", 4);

	shader->setBool(programName, "useSSAO", useSSAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}

void GraphicsPipeLine::PutExposure(const char* programName) {
	auto shader = Shader::getInstance();
	auto progrma = shader->getShaderProgram(programName);
	glUseProgram(progrma);
	shader->setFloat(programName, "exposure", exposure);
}


void GraphicsPipeLine::CreateLigtingFramebuffer() {
	glGenFramebuffers(1, &lightingFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lightingFrameBuffer);

	glGenTextures(1, &lightingTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, lightingTexture);
	lightingTexture = CreateSimpleTexture();
	glBindTexture(GL_TEXTURE_2D, lightingTexture);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		lightingTexture,
		0
	);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(
		GL_RENDERBUFFER, 
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

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GraphicsPipeLine::CreateVAO()
{
	SimpleQuad result = CreateSimpleQuad();

	vao = result.VAO;
	vbo = result.VBO;
}

void GraphicsPipeLine::DrawGBuffer(shared_ptr<MeshRenderer> mesh) {
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(gBuffer->programName);
	glUseProgram(program);
	gBuffer->use();
	shader->setBool(gBuffer->programName, "isGodRay", false);
	mesh->Draw(gBuffer->programName);

	gBuffer->UpdateImGui();
}

void GraphicsPipeLine::DrawGBuffer(
	shared_ptr<MeshRenderer> mesh,
	glm::vec3 lightPos
) {
	auto shader = Shader::getInstance();

	DrawGBuffer(mesh);
	auto program = shader->getShaderProgram(gBuffer->programName);
	glUseProgram(program);
	shader->setBool(gBuffer->programName, "isGodRay", true);
	godRays->rayCircle->position = lightPos;
	godRays->rayCircle->Draw(gBuffer->programName);

	gBuffer->UpdateImGui();
}

void GraphicsPipeLine::DrawSSAO() {
	ssao->DrawSSAO(gBuffer->positionMetallicTexture, gBuffer->normalTexture);
}

void GraphicsPipeLine::DefferedLighting() {
	const char* programName = "deffered";

	auto shader = Shader::getInstance();
	glUseProgram(shader->getShaderProgram(programName));
	glBindFramebuffer(GL_FRAMEBUFFER, lightingFrameBuffer);
	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0 + 13);
	glBindTexture(GL_TEXTURE_2D, gBuffer->positionMetallicTexture);
	shader->setInt(programName, "positionMetallicTexture", 13);

	glActiveTexture(GL_TEXTURE0 + 9);
	glBindTexture(GL_TEXTURE_2D, gBuffer->albedoRoughnessTexture);
	shader->setInt(programName, "albedoRoughnessTexture", 9);

	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_2D, gBuffer->normalTexture);
	shader->setInt(programName, "normalTexture", 11);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->gBufferFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightingFrameBuffer);
	// 뎁스버퍼 복사.
	glBlitFramebuffer(
		0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		0, 0,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
}

void GraphicsPipeLine::UpdateImGui() {
	useSSAO = EditorSharedValue::useSSAO;
}
