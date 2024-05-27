#include "GodRays.h"
#include "../Constants.h"
#include "../Editor/EditorSharedValue.h"

GodRays::GodRays() {
	// 텍스처 생성
	glGenTextures(1, &godRayTexture);
	glBindTexture(GL_TEXTURE_2D, godRayTexture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB8,
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


	// 프레임버퍼 생성
	glGenFramebuffers(1, &godRayFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, godRayFrameBuffer);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		godRayTexture,
		0
	);

	// vao 생성
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), &quad[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	// rbo 생성
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// godRay에 있을 원형 구체.

	rayCircle = std::make_shared<Circle>();
	rayCircle->scale = glm::vec3(2.0f);
	rayCircle->SetupMesh();

	EditorSharedValue::godLightTexture = godRayTexture;
	EditorSharedValue::decay = decay;
	EditorSharedValue::density = density;
	EditorSharedValue::weight = weight;
	EditorSharedValue::godRayExposure = exposure;
}

void GodRays::ImGuiUpdate() {
	decay = EditorSharedValue::decay;
	density = EditorSharedValue::density;
	weight = EditorSharedValue::weight;
	exposure = EditorSharedValue::godRayExposure;
}

void GodRays::Draw(
	shared_ptr<MeshRenderer> mesh,
	glm::vec3 rayPosition,
	std::shared_ptr<Camera> camera,
	unsigned int godRaySceneTexture) {
	const char* rayEffect = "god-ray-effect";
	glBindFramebuffer(GL_FRAMEBUFFER, godRayFrameBuffer);
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(rayEffect);
	glUseProgram(program);

	glm::vec2 screenPos = WorldToScreen(rayPosition, camera->view, camera->projection);

	// worldToScreen은 왼쪽상단이 0,0 오른쪽 하단이 WINDOW_WIDTH, WINDOW_HEIGHT 좌표계
	// 이를 opengl 텍스처 좌표계로 변환해야함.
	screenPos.x = screenPos.x / WINDOW_WIDTH;
	screenPos.y = 1.0 - (screenPos.y / WINDOW_HEIGHT);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGuiUpdate();
	shader->setFloat(rayEffect, "decay", decay);
	shader->setFloat(rayEffect, "density", density);
	shader->setFloat(rayEffect, "weight", weight);
	shader->setFloat(rayEffect, "exposure", exposure);

	shader->setVec2(rayEffect, "lightPosition", screenPos);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, godRaySceneTexture);
	shader->setInt(rayEffect, "godRayTexture", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GodRays::~GodRays() {
	glDeleteFramebuffers(1, &godRayFrameBuffer);
	glDeleteTextures(1, &godRayTexture);
	glDeleteRenderbuffers(1, &rbo);
}
