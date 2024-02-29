#pragma once
class DummyTexture2D
{
private:
	static DummyTexture2D* instance;
public:
	DummyTexture2D();

	unsigned int dummyTexture2D;

	static DummyTexture2D* getInstance() {
		if (instance == nullptr) {
			instance = new DummyTexture2D();
		}
		return instance;
	}
};

