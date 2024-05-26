#pragma once
class MainSceneTexture
{
private:
	unsigned int mainSceneTextureFrameBuffer;
	unsigned int mainSceneTextureId;

public:
	MainSceneTexture();

	unsigned int getMainSceneTexture() const {
		return mainSceneTextureId;
	}

	unsigned int getMainSceneFrameBuffer() const {
		return mainSceneTextureFrameBuffer;
	}
};

