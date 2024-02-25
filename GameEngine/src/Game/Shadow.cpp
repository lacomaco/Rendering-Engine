#include "Shadow.h"
#include <glew.h>

Shadow::Shadow() {
	glGenFramebuffers(1, &depthMapFrameBuffer);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		SHADOW_WIDTH,
		SHADOW_HEIGHT,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST
	);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST
	);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		GL_REPEAT
	);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		GL_REPEAT
	);

	glBindFramebuffer(
		GL_FRAMEBUFFER,
		depthMapFrameBuffer
	);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		depthMap,
		0
	);

	// 색상버퍼 쓰기 비활성화
	glDrawBuffer(GL_NONE);
	// 깊이버퍼 읽기 비활성화.
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shadow::WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, Camera* camera) {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFrameBuffer);

	glClear(GL_DEPTH_BUFFER_BIT);
	meshRenderer->Draw("shadow",camera);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
