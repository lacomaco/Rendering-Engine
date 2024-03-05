#include "Bloom.h"
#include "../Util/GLHelper.h"
#include <glew.h>
#include "../Constants.h"

Bloom::Bloom() {

	auto quad = CreateSimpleQuad();

	vao = quad.VAO;
	vbo = quad.VBO;

	CreateSceneTexture();
}

void Bloom::CreateSceneTexture() {
	glGenTextures(1, &sceneTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, sceneTexture);
	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE,
		4,
		GL_RGBA32F,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		GL_TRUE
	);
}
