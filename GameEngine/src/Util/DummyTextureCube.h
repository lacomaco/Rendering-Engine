#pragma once
#include <iostream>
class DummyTextureCube
{
private:
	static DummyTextureCube* instance;
public:
	DummyTextureCube();
	unsigned int dummyTextureCube;

	static DummyTextureCube* getInstance() {
		if (instance == nullptr) {
			instance = new DummyTextureCube();
		}
		return instance;
	}
};

