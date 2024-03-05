#pragma once
class Bloom
{
public:
	Bloom();

	unsigned int vao;
	unsigned int vbo;
	unsigned int sceneTexture;
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	int bloomCount = 10;

	void CreateSceneTexture();
	void CopySceneTexture(unsigned int msaaFB);

};

