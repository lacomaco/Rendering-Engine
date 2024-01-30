#include "GlobalMapComponent.h"
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <iostream>
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
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(0.0f, 10.0f);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			auto& node = mapGraph[i][j];

			auto left = map[i][j][0];
			auto up = map[i][j][1];
			auto right = map[i][j][2];
			auto down = map[i][j][3];

			if (left == 0) {
				Edge edge = { mapGraph[i][j], mapGraph[i][j - 1], dist(rng)};
				node->adjacentEdges.push_back(edge);
			}

			if (up == 0) {
				Edge edge = { mapGraph[i][j], mapGraph[i - 1][j], dist(rng)};
				node->adjacentEdges.push_back(edge);
			}

			if (right == 0) {
				Edge edge = { mapGraph[i][j], mapGraph[i][j + 1], dist(rng)};
				node->adjacentEdges.push_back(edge);
			}

			if (down == 0) {
				Edge edge = { mapGraph[i][j], mapGraph[i + 1][j], dist(rng)};
				node->adjacentEdges.push_back(edge);
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

void GlobalMapComponent::GetShortestPath(int startX, int startY, int endX, int endY) {
	std::unordered_set<std::shared_ptr<GraphNode>> checkNodeMap;

	std::shared_ptr<GraphNode> startNode = mapGraph[startX][startY];
	std::shared_ptr<GraphNode> endNode = mapGraph[endX][endY];

	bool pathFound = false;
	std::queue<std::shared_ptr<GraphNode>> q;
	std::queue<std::shared_ptr<TraceGraph>> tq;
	std::shared_ptr<TraceGraph> trace = std::make_shared<TraceGraph>();
	trace->current = startNode;



	q.push(startNode);
	tq.push(trace);


	checkNodeMap.insert(startNode);

	while (!q.empty()) {
		auto& current = q.front();
		std::shared_ptr<TraceGraph> currentTrace = tq.front();

		q.pop();
		tq.pop();


		if (endNode == current) {
			pathFound = true;
			// 스택사용 뒤로가기.
			std::stack<std::shared_ptr<TraceGraph>> st;

			std::shared_ptr<TraceGraph> shortestPath;

			while (currentTrace != nullptr) {
				st.push(currentTrace);
				currentTrace = currentTrace->parent;
			}

			while (!st.empty()) {
				auto& current = st.top();
				st.pop();

				if (!st.empty()) {
					current->childNode = st.top();
				}
			}

			traceGraph = trace;

			break;
		}

		for (const auto& edge : current->adjacentEdges) {
			auto check = checkNodeMap.find(edge.to);

			if (check != checkNodeMap.end()) {
				continue;
			}

			checkNodeMap.insert(edge.to);


			std::shared_ptr<TraceGraph> childTrace = std::make_shared<TraceGraph>();
			childTrace->current = edge.from;
			childTrace->parent = currentTrace;

			q.emplace(edge.to);
			tq.emplace(childTrace);
		}

	}
}
void GlobalMapComponent::GreedyBestFirstSearch(int startX, int startY, int endX, int endY) {
	std::shared_ptr<GraphNode> startNode = mapGraph[startX][startY];
	std::shared_ptr<GraphNode> endNode = mapGraph[endX][endY];

}

void GlobalMapComponent::RenderMaze(SDL_Renderer* renderer) {
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

void GlobalMapComponent::RenderShortestPathStepByStep(SDL_Renderer* renderer) {
	if (!traceGraph) {
		return;
	}

	auto& currentNode = traceGraph->current;

	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	const int startX = currentNode->x * CELL_SIZE;
	const int startY = currentNode->y * CELL_SIZE;
	
	SDL_Rect rect = {
		startX,
		startY,
		CELL_SIZE,
		CELL_SIZE
	};

	SDL_RenderFillRect(renderer, &rect);

	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	if (traceGraph->childNode) {
		traceGraph = traceGraph->childNode;
	}
}
