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

void SurfaceQuadNode::setNeighbours()
{
	switch (type)
	{
	case QUAD_CHILD::TOP_LEFT:
		if (parent->neighbourLeft != NULL)
		{
			if (parent->neighbourLeft->plane != plane)
			{
				neighbourLeft = getNeareastNeighbour(parent->neighbourLeft, parent->corners.p0.get());
			}
			else
			{
				neighbourLeft = parent->neighbourLeft->topRight;
			}
		}

		if (parent->neighbourTop != NULL)
		{
			if(parent->neighbourTop->plane != plane)
			{
				neighbourTop = getNeareastNeighbour(parent->neighbourTop, parent->corners.p0.get());
			}
			else
			{
				neighbourTop = parent->neighbourTop->botLeft;
			}
		}
		break;
	case QUAD_CHILD::BOT_LEFT:
		if (parent->neighbourLeft != NULL)
		{
			if (parent->neighbourLeft->plane != plane)
			{
				neighbourLeft = getNeareastNeighbour(parent->neighbourLeft, parent->corners.p1.get());
			}
			else
			{
				neighbourLeft = parent->neighbourLeft->botRight;
			}
		}

		if (parent->neighbourBot != NULL)
		{
			if(parent->neighbourBot->plane != plane)
			{
				neighbourBot = getNeareastNeighbour(parent->neighbourBot, parent->corners.p1.get());
			}
			else
			{
				neighbourBot = parent->neighbourBot->topLeft;
			}
		}

		neighbourTop = parent->topLeft;
		parent->topLeft->neighbourBot = this;
		break;
	case QUAD_CHILD::TOP_RIGHT:
		if (parent->neighbourTop != NULL)
		{
			if (parent->neighbourTop->plane != plane)
			{
				neighbourTop = getNeareastNeighbour(parent->neighbourTop, parent->corners.p2.get());
			}
			else
			{
				neighbourTop = parent->neighbourTop->botRight;
			}
		}

		if(parent->neighbourRight != NULL)
		{
			if(parent->neighbourRight->plane != plane)
			{
				neighbourRight = getNeareastNeighbour(parent->neighbourRight, parent->corners.p2.get());
			}
			else
			{
				neighbourRight = parent->neighbourRight->topLeft;
			}
		}

		neighbourLeft = parent->topLeft;
		parent->topLeft->neighbourRight = this;
		break;
	case QUAD_CHILD::BOT_RIGHT:
		if(parent->neighbourBot != NULL)
		{
			if(parent->neighbourBot->plane != plane)
			{
				neighbourBot = getNeareastNeighbour(parent->neighbourBot, parent->corners.p3.get());
			}
			else
			{
				neighbourBot = parent->neighbourBot->topRight;
			}
		}

		if (parent->neighbourRight != NULL)
		{
			if (parent->neighbourRight->plane != plane)
			{
				neighbourRight = getNeareastNeighbour(parent->neighbourRight, parent->corners.p3.get());
			}
			else
			{
				neighbourRight = parent->neighbourRight->botLeft;
			}
		}

		neighbourLeft = parent->botLeft;
		parent->botLeft->neighbourRight = this;

		neighbourTop = parent->topRight;
		parent->topRight->neighbourBot = this;
		break;
	}
}

void SurfaceQuadNode::initVertices()
{
	float x, y, z;
	
	switch (type)
	{
	case QUAD_CHILD::TOP_LEFT:
		corners.p0 = parent->corners.p0;

		x = (parent->corners.p0->coords.x + parent->corners.p1->coords.x) / 2;
		y = (parent->corners.p0->coords.y + parent->corners.p1->coords.y) / 2;
		z = (parent->corners.p0->coords.z + parent->corners.p1->coords.z) / 2;
		if(neighbourLeft != NULL)
		{
			corners.p1 = getNeededNeighbourPoint(neighbourLeft, Point(x, y, z, 0));
		}
		else
		{
			corners.p1 = std::shared_ptr<Point>(new Point(x, y, z, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p1->coords);
		}

		x = (parent->corners.p0->coords.x + parent->corners.p2->coords.x) / 2;
		y = (parent->corners.p0->coords.y + parent->corners.p2->coords.y) / 2;
		z = (parent->corners.p0->coords.z + parent->corners.p2->coords.z) / 2;
		if(neighbourTop != NULL)
		{
			corners.p2 = getNeededNeighbourPoint(neighbourTop, Point(x, y, z, 0));
		}
		else
		{
			corners.p2 = std::shared_ptr<Point>(new Point(x, y, z, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p2->coords);
		}

		x = (parent->corners.p0->coords.x + parent->corners.p3->coords.x) / 2;
		y = (parent->corners.p0->coords.y + parent->corners.p3->coords.y) / 2;
		z = (parent->corners.p0->coords.z + parent->corners.p3->coords.z) / 2;
		corners.p3 = std::shared_ptr<Point>(new Point(x, y, z, planet->vertexBufferIndex++));
		planet->vertices.push_back(corners.p3->coords);
		break;
	case QUAD_CHILD::BOT_LEFT:
		corners.p0 = parent->topLeft->corners.p1;
		corners.p1 = parent->corners.p1;
		corners.p2 = neighbourTop->corners.p3;

		x = (parent->corners.p1->coords.x + parent->corners.p3->coords.x) / 2;
		y = (parent->corners.p1->coords.y + parent->corners.p3->coords.y) / 2;
		z = (parent->corners.p1->coords.z + parent->corners.p3->coords.z) / 2;
		if(neighbourBot != NULL)
		{
			corners.p3 = getNeededNeighbourPoint(neighbourBot, Point(x, y, z, 0));
		}
		else
		{
			corners.p3 = std::shared_ptr<Point>(new Point(x, y, z, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p3->coords);
		}
		break;
	case QUAD_CHILD::TOP_RIGHT:
		corners.p0 = neighbourLeft->corners.p2;
		corners.p1 = neighbourLeft->corners.p3;
		corners.p2 = parent->corners.p2;

		x = (parent->corners.p2->coords.x + parent->corners.p3->coords.x) / 2;
		y = (parent->corners.p2->coords.y + parent->corners.p3->coords.y) / 2;
		z = (parent->corners.p2->coords.z + parent->corners.p3->coords.z) / 2;
		if (neighbourRight != NULL)
		{
			corners.p3 = getNeededNeighbourPoint(neighbourRight, Point(x, y, z, 0));
		}
		else
		{
			corners.p3 = std::shared_ptr<Point>(new Point(x, y, z, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p3->coords);
		}
		break;
	case QUAD_CHILD::BOT_RIGHT:
		corners.p0 = neighbourTop->corners.p1;
		corners.p1 = neighbourLeft->corners.p3;
		corners.p2 = neighbourTop->corners.p3;
		corners.p3 = parent->corners.p3;
		break;
	}
}
