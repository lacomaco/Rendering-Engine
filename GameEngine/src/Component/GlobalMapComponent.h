#pragma once
#include <vector>
#include <SDL.h>
#include <memory>
#include <queue>

/*
* 보통 컴포넌트는 독립적이지만 Global 컴포넌트는 독립적이지 않다.
* 따라서 register를 통해서 entity에 등록하는 과정도 필요하지 않다.
* 그냥 싱글톤으로 이용한다.
* 
*/

struct Edge;

struct GraphNode {
	int x;
	int y;
	int GraphNodeIndex;
	std::vector<Edge> adjacentEdges;
};

struct Edge {
	std::shared_ptr<GraphNode> from;
	std::shared_ptr<GraphNode> to;
	float weight;

	Edge(std::shared_ptr<GraphNode> from, std::shared_ptr<GraphNode> to, float weight) {
		this->from = from;
		this->to = to;
		this->weight = weight;
	}
};

struct TraceGraph {
	std::shared_ptr<TraceGraph> parent;
	std::shared_ptr<GraphNode> current;
	std::shared_ptr<TraceGraph> childNode;
};

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
	~GlobalMapComponent();

	// 3차원 자료구조
	// map[x][y][0] : 왼쪽벽
	// map[x][y][1] : 윗벽
	// map[x][y][2] : 오른쪽벽
	// map[x][y][3] : 아래벽
	std::vector<std::vector<std::vector<int>>> map;
	std::vector<std::vector<std::shared_ptr<GraphNode>>> mapGraph;
	void GetShortestPath(int startX, int startY, int endX, int endY);
	void RenderShortestPathStepByStep(SDL_Renderer* renderer);
	void GreedyBestFirstSearch(int startX, int startY, int endX, int endY);

	std::shared_ptr<TraceGraph> traceGraph;
};