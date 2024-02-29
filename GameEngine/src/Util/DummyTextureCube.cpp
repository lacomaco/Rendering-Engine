#include "DummyTextureCube.h"
#include <glew.h>
#include <iostream>

DummyTextureCube* DummyTextureCube::instance = nullptr;

DummyTextureCube::DummyTextureCube() {
	glGenTextures(1, &dummyTextureCube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dummyTextureCube);
	unsigned char data[] = { 255,255,255,255 };
	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}