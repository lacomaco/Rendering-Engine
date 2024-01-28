#pragma once
#include <vector>
#include <memory>
#include <SDL.h>

struct QuadTreeRectangle {
	int x;
	int y;
	int width;
	int height;
	int entityId;

	QuadTreeRectangle(int x = 0, int y = 0, int width = 0, int height = 0, int entityId = 0) {
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->entityId = entityId;
	}
};

class QuadTree
{
private:
	const int MAX_OBJECTS = 4;
	const int MAX_LEVELS = 8;
	QuadTreeRectangle bounds;
	/*
	* 0: top right
	* 1: top left
	* 2: bottom left
	* 3: bottom right
	* 	*/
	std::vector<std::unique_ptr<QuadTree>> nodes;

	int level;
	std::vector<QuadTreeRectangle> objects;

public:
	QuadTree(QuadTreeRectangle pBounds, int pLevel=0);
	~QuadTree();
	void Clear();
	void Split();
	int GetIndex(QuadTreeRectangle pRect);
	void Insert(QuadTreeRectangle pRect);
	std::vector<QuadTreeRectangle>& Retrieve(std::vector<QuadTreeRectangle>& returnObjects, QuadTreeRectangle pRect);

	void DrawBounds(SDL_Renderer* renderer);
};

