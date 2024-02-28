#include "PointShadow.h"
#include "glew.h"
#include <iostream>
#include "../Util/Shader.h"

PointShadow::PointShadow()
{
	glGenTextures(1,&depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (int i = 0; i < 6; i++) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_DEPTH_COMPONENT,
			SHADOW_WIDTH,
			SHADOW_HEIGHT,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			NULL
		);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void PointShadow::WriteDepthMap(shared_ptr<MeshRenderer> meshRenderer, std::vector<glm::mat4> lightSpaceMatrix)
{

	auto shader = Shader::getInstance();

	for (auto& matrix : lightSpaceMatrix)
	{
		shader->setMat4("point-shadow", "shadowMatrices", matrix);
	}

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		meshRenderer->Draw("point-shadow");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}