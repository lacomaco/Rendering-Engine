#pragma once
#include <vector>
#include <SDL.h>

class GlobalMapComponent {
private:
	static GlobalMapComponent* globalMapComponent;
	int width;
	int height;

	void GenerateMazeByBinaryAlgorithm();
	void GenerateTreePathBasedMaze();

public:
	static const int CELL_SIZE = 30;

	static GlobalMapComponent* GetInstance(int width, int height) {

		if (globalMapComponent == nullptr) {
			globalMapComponent = new GlobalMapComponent(width, height);
		}
		return globalMapComponent;
	}

	void RenderMaze(SDL_Renderer* renderer);

	GlobalMapComponent(int width, int height);

	// 3차원 자료구조
	// map[x][y][0] : 왼쪽벽
	// map[x][y][1] : 윗벽
	// map[x][y][2] : 오른쪽벽
	// map[x][y][3] : 아래벽
	std::vector<std::vector<std::vector<int>>> map;
};