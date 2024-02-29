#include "DummyTexture2D.h"
#include <iostream>
#include <glew.h>

DummyTexture2D* DummyTexture2D::instance = nullptr;

DummyTexture2D::DummyTexture2D() {
	glGenTextures(1, &dummyTexture2D);
	glBindTexture(GL_TEXTURE_2D, dummyTexture2D);
	unsigned char data[] = { 255,255,255,255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}