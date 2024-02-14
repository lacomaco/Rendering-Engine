#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include "../Game/Camera.h"


class Input
{
private:
	static Input* instance;
	Input();
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
public:
	static Input* GetInstance() {
		if (instance == nullptr) {
			instance = new Input();
		}

		return instance;
	}

	const Uint8* state;

	bool IsKeyPressed(int key) {
		return state[key];
	}

	bool rightMouseButtonDown = false;
	/*
	* 실제 윈도우 좌표계는 왼쪽 상단이 (0, 0) 이지만
	* 
	* 편의상 Input 클래스에서 마우스 좌표계는 NDC와 동일하다.
	*/
	glm::vec2 mousePos;
	glm::vec2 ScreenNormalize(Sint32 x,Sint32 y);
	void CameraLookAround(Sint32 x,Sint32 y, Camera* camera);
	void SetMouse(Sint32 x, Sint32 y);
};

