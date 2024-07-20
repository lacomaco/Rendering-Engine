#include "IconTexture.h"
#include <glew.h>
#include "../Util/stb_image.h"
#include <iostream>

IconTexture::IconTexture(const char* filename) {
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	GLenum format;
	GLenum internalFormat;
	if (nrChannels == 1) {
		format = GL_RED;
		internalFormat = GL_R8;
	}
	else if (nrChannels == 3) {
		format = GL_RGB;
		internalFormat = GL_RGB8;
	}
	else if (nrChannels == 4) {
		format = GL_RGBA;
		internalFormat = GL_RGBA8;
	}
	else {
		std::cerr << "Unsupported number of channels: " << nrChannels<< " " << filename << std::endl;
		stbi_image_free(data);
		return;
	}

	if (data) {
		glTextureStorage2D(textureID, 1, internalFormat, width, height);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL Error: after Storage 2D" << error << std::endl;
		}

		glTextureSubImage2D(
			textureID,
			0,0,0,
			width,
			height,
			format,
			GL_UNSIGNED_BYTE,
			data
		);


		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL Error: After Sub Image 2D " << error << std::endl;
		}

	}

	stbi_image_free(data);
}
