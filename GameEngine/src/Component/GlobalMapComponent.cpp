#include "GlobalMapComponent.h"
#include <random>

GlobalMapComponent* GlobalMapComponent::globalMapComponent = nullptr;

GlobalMapComponent::GlobalMapComponent(int width, int height) {
	this->width = width;
	this->height = height;
	map.resize(width);

	for (int i = 0; i < width; i++) {
		map[i].resize(height);
		for (int j = 0; j < height; j++) {
			map[i][j].resize(4);
			map[i][j][0] = 1;
			map[i][j][1] = 1;
			map[i][j][2] = 1;
			map[i][j][3] = 1;
		}
	}

	GenerateMazeByBinaryAlgorithm();

	mapGraph.resize(width);
	for (int i = 0; i < width; i++) {
		mapGraph[i].resize(height);
		for (int j = 0; j < height; j++) {
			GraphNode node = { i, j, i * width + j };
			mapGraph[i][j] = std::make_shared<GraphNode>(node);
		}
	}

	GenerateTreePathBasedMaze();
}

void GlobalMapComponent::GenerateTreePathBasedMaze() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			auto& node = mapGraph[i][j];

			auto left = map[i][j][0];
			auto up = map[i][j][1];
			auto right = map[i][j][2];
			auto down = map[i][j][3];

			if (left == 0) {
				node->adjacentNodes.push_back(mapGraph[i][j-1]);
			}

			if (up == 0) {
				node->adjacentNodes.push_back(mapGraph[i-1][j]);
			}

			if (right == 0) {
				node->adjacentNodes.push_back(mapGraph[i][j+1]);
			}

			if (down == 0) {
				node->adjacentNodes.push_back(mapGraph[i+1][j]);
			}
		}
	}
}


GlobalMapComponent::~GlobalMapComponent() {
	delete globalMapComponent;
}

void GlobalMapComponent::GenerateMazeByBinaryAlgorithm() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, 1);
	// 3차원 자료구조
	// map[x][y][0] : 왼쪽벽
	// map[x][y][1] : 윗벽
	// map[x][y][2] : 오른쪽벽
	// map[x][y][3] : 아래벽
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i == 0 && j == 0) {
				map[i][j][2] = 0;
				map[i][j][3] = 0;
				continue;
			}

			if (i == 0) {
				map[i][j][0] = 0;
				map[i][j - 1][2] = 0;
				continue;
			}

			if (j == 0) {
				map[i][j][1] = 0;
				map[i - 1][j][3] = 0;
				continue;
			}

			const double randomValue = dis(gen);
			if (randomValue < 0.5) {
				map[i][j][0] = 0;
				map[i][j - 1][2] = 0;
			}
			else {
				map[i][j][1] = 0;
				map[i - 1][j][3] = 0;
			}
		}
	}
}

void GlobalMapComponent::RenderMaze(SDL_Renderer* renderer) {
	{
		Uint8 r, g, b, a;
		SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				const int startX = i * CELL_SIZE;
				const int startY = j * CELL_SIZE;

				// 왼쪽 벽
				if (map[i][j][0] == 1) {
					SDL_RenderDrawLine(renderer, startX, startY, startX + CELL_SIZE, startY);
				}

				// 윗 벽
				if (map[i][j][1] == 1) {
					SDL_RenderDrawLine(renderer, startX, startY, startX, startY + CELL_SIZE);
				}

				// 오른쪽 벽
				if (map[i][j][2] == 1) {
					SDL_RenderDrawLine(renderer, startX, startY + CELL_SIZE, startX + CELL_SIZE, startY + CELL_SIZE);
				}

				// 아래쪽 벽
				if (map[i][j][3] == 1) {
					SDL_RenderDrawLine(renderer, startX + CELL_SIZE, startY, startX + CELL_SIZE, startY + CELL_SIZE);
				}
			}
		}


		SDL_SetRenderDrawColor(renderer, r, g, b, a);
	}
}
