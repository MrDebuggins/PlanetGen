#include "SurfaceQuadTree.h"


void SurfaceQuadNode::pushElemntIndexes()
{
	if ((depth == planet->currentDepth) || (topLeft == NULL))
	{
		planet->primitives.push_back(corners.p0->index);
		planet->primitives.push_back(corners.p1->index);
		planet->primitives.push_back(corners.p2->index);

		planet->primitives.push_back(corners.p1->index);
		planet->primitives.push_back(corners.p2->index);
		planet->primitives.push_back(corners.p3->index);
	}
	else if(depth < planet->currentDepth)
	{
		topLeft->pushElemntIndexes();
		botLeft->pushElemntIndexes();
		topRight->pushElemntIndexes();
		botRight->pushElemntIndexes();
	}
}

std::shared_ptr<Point> SurfaceQuadNode::getNeededNeighbourPoint(SurfaceQuadNode* neighbour, Point p)
{
	if (*(neighbour->corners.p0) == p)
		return neighbour->corners.p0;

	if (*(neighbour->corners.p1) == p)
		return neighbour->corners.p1;

	if (*(neighbour->corners.p2) == p)
		return neighbour->corners.p2;

	return neighbour->corners.p3;
}

double SurfaceQuadNode::getDistanceBetweenNodeAndPoint(SurfaceQuadNode* a, Point* c2)
{
	Point c1 = Point((a->corners.p0->coords.x + a->corners.p3->coords.x) / 2, (a->corners.p0->coords.y + a->corners.p3->coords.y) / 2, (a->corners.p0->coords.z + a->corners.p3->coords.z) / 2, 0);

	return sqrt((c1.coords.x - c2->coords.x)* (c1.coords.x - c2->coords.x) + (c1.coords.y - c2->coords.y)* (c1.coords.y - c2->coords.y) + (c1.coords.z - c2->coords.z)* (c1.coords.z - c2->coords.z));
}

SurfaceQuadNode* SurfaceQuadNode::getNeareastNeighbour(SurfaceQuadNode* neighboursParent, Point* current)
{
	if (neighboursParent->botLeft == NULL)
	{
		return NULL;
	}

	SurfaceQuadNode* nearest = neighboursParent->topLeft;

	double minDistance = getDistanceBetweenNodeAndPoint(neighboursParent->topLeft, current);

	double tmp = getDistanceBetweenNodeAndPoint(neighboursParent->botLeft, current);

	if (tmp < minDistance)
	{
		minDistance = tmp;
		nearest = neighboursParent->botLeft;
	}

	tmp = getDistanceBetweenNodeAndPoint(neighboursParent->topRight, current);

	if (tmp < minDistance)
	{
		minDistance = tmp;
		nearest = neighboursParent->topRight;
	}

	tmp = getDistanceBetweenNodeAndPoint(neighboursParent->botRight, current);

	if (tmp < minDistance)
	{
		minDistance = tmp;
		nearest = neighboursParent->botRight;
	}

	return nearest;
}