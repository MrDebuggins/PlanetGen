#pragma once

#include "PlanetProperties.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>


struct Point
{
	glm::vec3 coords;
	unsigned int index;

	Point(float x, float y, float z, unsigned int index)
	{
		coords = glm::vec3(x, y, z);
		this->index = index;
	}

	bool operator==(Point const obj)
	{
		if (this->coords.x == obj.coords.x && this->coords.y == obj.coords.y && this->coords.z == obj.coords.z)
		{
			return true;
		}

		return false;
	}
};

struct QuadData
{
	std::shared_ptr<Point> p0;
	std::shared_ptr<Point> p1;
	std::shared_ptr<Point> p2;
	std::shared_ptr<Point> p3;

	~QuadData()
	{
		p0.~shared_ptr();
		p1.~shared_ptr();
		p2.~shared_ptr();
		p3.~shared_ptr();
	}
};

enum class QUAD_PLANE
{
	XY,
	XZ,
	YZ
};

enum class QUAD_CHILD
{
	ROOT,
	TOP_LEFT,
	BOT_LEFT,
	TOP_RIGHT,
	BOT_RIGHT,
};

class SurfaceQuadNode
{
public:
	PlanetProperties* planet = NULL;

	QUAD_CHILD type;
	QuadData corners;

	SurfaceQuadNode* parent;

	QUAD_PLANE plane = QUAD_PLANE::XY;
	float planeDirection;

	unsigned int depth;// 0 - root

	SurfaceQuadNode *topLeft = NULL;
	SurfaceQuadNode *botLeft = NULL;
	SurfaceQuadNode *topRight = NULL;
	SurfaceQuadNode *botRight = NULL;

	SurfaceQuadNode* neighbourTop = NULL;
	SurfaceQuadNode* neighbourLeft = NULL;
	SurfaceQuadNode* neighbourBot = NULL;
	SurfaceQuadNode* neighbourRight = NULL;

	static void DestroyQuadTree(SurfaceQuadNode* node) 
	{
		if (node)
		{
			DestroyQuadTree(node->botRight);
			DestroyQuadTree(node->topRight);
			DestroyQuadTree(node->botLeft);
			DestroyQuadTree(node->topLeft);

			delete node;
		}
	}

	~SurfaceQuadNode(){}

	virtual void split() = 0;

	virtual void setNeighbours() = 0;

	virtual void initVertices() = 0;

	std::shared_ptr<Point> getNeededNeighbourPoint(SurfaceQuadNode* neighbour, Point p);

	double getDistanceBetweenNodeAndPoint(SurfaceQuadNode* a, Point* c2);

	SurfaceQuadNode* getNeareastNeighbour(SurfaceQuadNode* neighboursParent, Point* current);

	void pushElemntIndexes();
};