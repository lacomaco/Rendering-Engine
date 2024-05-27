#pragma once

class IconTexture
{
private:
	unsigned int textureID;

public:
	IconTexture(const char* filename);

	unsigned int getTexture() const {
		return textureID;
	}
};

