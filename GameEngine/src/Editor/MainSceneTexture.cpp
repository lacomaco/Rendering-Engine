#include "MainSceneTexture.h"
#include <glew.h>
#include "../Util/GLHelper.h"


MainSceneTexture::MainSceneTexture() {
	glGenFramebuffers(1, &mainSceneTextureFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mainSceneTextureFrameBuffer);

	mainSceneTextureId = CreateSimpleTexture();

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		mainSceneTextureId,
		0
	);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}