#include "GodRays.h"
#include "../Constants.h"

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

	glGenTextures(1, &godRaySceneTexture);
	glBindTexture(GL_TEXTURE_2D, godRaySceneTexture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0,
		GL_RGBA,
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
		godRaySceneTexture,
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
}

void GodRays::Draw(
	shared_ptr<MeshRenderer> mesh,
	glm::vec3 rayPosition,
	std::shared_ptr<Camera> camera) {
	const char* godRayProgramName = "god-ray";
	glBindFramebuffer(GL_FRAMEBUFFER, godRayFrameBuffer);
	// rbo도 넣어야할까? 고민해보자.
	auto shader = Shader::getInstance();
	auto program = shader->getShaderProgram(godRayProgramName);
	glUseProgram(program);
	
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	camera->putCameraUniform(godRayProgramName);
	shader->setVec3(godRayProgramName, "rayPosition", rayPosition);
	shader->setBool(godRayProgramName, "isGodRay", false);


	mesh->Draw(godRayProgramName);

	shader->setBool(godRayProgramName, "isGodRay", true);
	rayCircle->position = rayPosition;
	rayCircle->Draw(godRayProgramName);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GodRays::~GodRays() {
	glDeleteFramebuffers(1, &godRayFrameBuffer);
	glDeleteTextures(1, &godRayTexture);
	glDeleteRenderbuffers(1, &rbo);
}
