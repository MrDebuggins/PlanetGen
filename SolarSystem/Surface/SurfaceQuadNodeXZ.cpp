#include "SurfaceQuadNodeXZ.h"

SurfaceQuadNodeXZ::SurfaceQuadNodeXZ(float c1Min, float c2Min, float c1Max, float c2Max, QUAD_PLANE plane, float planeDirection, PlanetProperties* planet)
{
	type = QUAD_CHILD::ROOT;
	this->plane = plane;
	this->planeDirection = planeDirection;
	this->planet = planet;

	float y = planeDirection * planet->radius;
	if (planeDirection > 0)
	{
		corners.p0 = std::shared_ptr<Point>(new Point(c1Min, y, -planeDirection * c2Max, planet->vertexBufferIndex++));
		corners.p2 = std::shared_ptr<Point>(new Point(c1Max, y, -planeDirection * c2Max, planet->vertexBufferIndex++));
		
		planet->vertices.push_back(corners.p0->coords);
		planet->vertices.push_back(corners.p2->coords);
	}
	else 
	{
		corners.p1 = std::shared_ptr<Point>(new Point(c1Min, y, -planeDirection * c2Min, planet->vertexBufferIndex++));
		corners.p3 = std::shared_ptr<Point>(new Point(c1Max, y, -planeDirection * c2Min, planet->vertexBufferIndex++));
		planet->vertices.push_back(corners.p1->coords);
		planet->vertices.push_back(corners.p3->coords);
	}
}

SurfaceQuadNodeXZ::SurfaceQuadNodeXZ(SurfaceQuadNode* parent, QUAD_CHILD type, float planeDirection, PlanetProperties* planet)
{
	this->parent = parent;
	this->type = type;
	this->plane = QUAD_PLANE::XZ;
	this->planeDirection = parent->planeDirection;
	this->planet = planet;
	depth = parent->depth + 1;

	setNeighbours();
	initVertices();
}

void SurfaceQuadNodeXZ::split()
{
	if (topLeft == NULL)
	{
		topLeft = new SurfaceQuadNodeXZ(this, QUAD_CHILD::TOP_LEFT, planeDirection, planet);
		botLeft = new SurfaceQuadNodeXZ(this, QUAD_CHILD::BOT_LEFT, planeDirection, planet);
		topRight = new SurfaceQuadNodeXZ(this, QUAD_CHILD::TOP_RIGHT, planeDirection, planet);
		botRight = new SurfaceQuadNodeXZ(this, QUAD_CHILD::BOT_RIGHT, planeDirection, planet);

		return;
	}

	if(depth < 2)
	{
		topLeft->split();
		botLeft->split();
		topRight->split();
		botRight->split();
	}
}

void SurfaceQuadNodeXZ::setNeighbours()
{
	switch (this->type)
	{
	case QUAD_CHILD::TOP_LEFT:
		if (parent->neighbourLeft != NULL)
		{
			if (parent->neighbourLeft->plane != plane)
			{
				float x = parent->corners.p0->coords.x + (parent->corners.p3->coords.x - parent->corners.p0->coords.x) / 4;
				float z = parent->corners.p0->coords.z + (parent->corners.p3->coords.z - parent->corners.p0->coords.z) / 4;
				Point c = Point(x, parent->corners.p0->coords.y, z, 0);
				neighbourLeft = getNeareastNeighbour(parent->neighbourLeft, &c);
			}
			else
			{
				neighbourLeft = parent->neighbourLeft->topRight;
			}
		}
			
		if (parent->neighbourTop != NULL)
			neighbourTop = parent->neighbourTop->botLeft;
		break;
	case QUAD_CHILD::BOT_LEFT:
		if (parent->neighbourLeft != NULL)
		{
			if (parent->neighbourLeft->plane != plane)
			{
				float x = parent->corners.p0->coords.x + (parent->corners.p3->coords.x - parent->corners.p0->coords.x) / 4;
				float z = parent->corners.p3->coords.z - (parent->corners.p3->coords.z - parent->corners.p0->coords.z) / 4;
				Point c = Point(x, parent->corners.p0->coords.y, z, 0);
				neighbourLeft = getNeareastNeighbour(parent->neighbourLeft, &c);
			}
			else
			{
				neighbourLeft = parent->neighbourLeft->botRight;
			}
		}

		if (parent->neighbourBot != NULL)
			neighbourBot = parent->neighbourBot->topLeft;

		neighbourTop = parent->topLeft;
		parent->topLeft->neighbourBot = this;
		break;
	case QUAD_CHILD::TOP_RIGHT:
		if (parent->neighbourTop != NULL)
			neighbourTop = parent->neighbourTop->botRight;

		if (parent->neighbourRight != NULL)
		{
			if (parent->neighbourRight->plane != plane)
			{
				float x = parent->corners.p3->coords.x - (parent->corners.p3->coords.x - parent->corners.p0->coords.x) / 4;
				float z = parent->corners.p0->coords.z + (parent->corners.p3->coords.z - parent->corners.p0->coords.z) / 4;
				Point c = Point(x, parent->corners.p0->coords.y, z, 0);
				neighbourRight = getNeareastNeighbour(parent->neighbourRight, &c);
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
		if (parent->neighbourBot != NULL)
			neighbourBot = parent->neighbourBot->topRight;

		if (parent->neighbourRight != NULL)
		{
			if (parent->neighbourRight->plane != plane)
			{
				float x = parent->corners.p3->coords.x - (parent->corners.p3->coords.x - parent->corners.p0->coords.x) / 4;
				float z = parent->corners.p3->coords.z - (parent->corners.p3->coords.z - parent->corners.p0->coords.z) / 4;
				Point c = Point(x, parent->corners.p0->coords.y, z, 0);
				neighbourRight = getNeareastNeighbour(parent->neighbourRight, &c);
			}
			else {
				neighbourRight = parent->neighbourRight->topLeft;
			}
		}
			

		neighbourLeft = parent->botLeft;
		neighbourTop = parent->topRight;

		parent->botLeft->neighbourRight = this;
		parent->topRight->neighbourBot = this;
		break;
	default:
		break;
	}
}

void SurfaceQuadNodeXZ::initVertices()
{
	float x1 = parent->corners.p0->coords.x;
	float x2 = parent->corners.p3->coords.x;
	float z1 = parent->corners.p0->coords.z;
	float z2 = parent->corners.p3->coords.z;

	switch (this->type)
	{
	case QUAD_CHILD::TOP_LEFT:
		corners.p0 = parent->corners.p0;

		if (neighbourLeft != NULL)
		{
			if (this->plane != neighbourLeft->plane)
			{
				corners.p1 = getNeededNeighbourPoint(neighbourLeft, Point(x1, planeDirection * planet->radius, z1 - (z1 - z2) / 2, 0));
			}
			else
			{
				corners.p1 = neighbourLeft->corners.p3;
			}
		}
		else
		{
			corners.p1 = std::shared_ptr<Point>(new Point(x1, planeDirection * planet->radius, z1 - (z1 - z2) / 2, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p1->coords);
		}

		if (neighbourTop != NULL)
		{
			corners.p2 = neighbourTop->corners.p3;
		}
		else
		{
			corners.p2 = std::shared_ptr<Point>(new Point(x1 + (x2 - x1) / 2, planeDirection * planet->radius, z1, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p2->coords);
		}

		corners.p3 = std::shared_ptr<Point>(new Point(x1 + (x2 - x1) / 2, planeDirection * planet->radius, z1 - (z1 - z2) / 2, planet->vertexBufferIndex++));
		planet->vertices.push_back(corners.p3->coords);
		break;
	case QUAD_CHILD::BOT_LEFT:
		corners.p0 = neighbourTop->corners.p1;
		corners.p1 = parent->corners.p1;
		corners.p2 = neighbourTop->corners.p3;

		if (neighbourBot != NULL)
		{
			corners.p3 = neighbourBot->corners.p2;
		}
		else
		{
			corners.p3 = std::shared_ptr<Point>(new Point(x1 + (x2 - x1) / 2, planeDirection * planet->radius, z2, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p3->coords);
		}
		break;
	case QUAD_CHILD::TOP_RIGHT:
		corners.p0 = neighbourLeft->corners.p2;
		corners.p1 = neighbourLeft->corners.p3;
		corners.p2 = parent->corners.p2;

		if (neighbourRight != NULL)
		{
			if (plane != neighbourRight->plane)
			{
				corners.p3 = getNeededNeighbourPoint(neighbourRight, Point(x2, planeDirection * planet->radius, z1 - (z1 - z2) / 2, 0));
			}
			else
			{
				corners.p3 = neighbourRight->corners.p1;
			}
		}
		else
		{
			corners.p3 = std::shared_ptr<Point>(new Point(x2, planeDirection * planet->radius, z1 - (z1 - z2) / 2, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p3->coords);
		}
		break;
	case QUAD_CHILD::BOT_RIGHT:
		corners.p0 = neighbourLeft->corners.p2;
		corners.p1 = neighbourLeft->corners.p3;
		corners.p2 = neighbourTop->corners.p3;
		corners.p3 = parent->corners.p3;
		break;
	default:
		break;
	}
}