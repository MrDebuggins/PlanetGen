#include "SurfaceQuadNodeXY.h"

SurfaceQuadNodeXY::SurfaceQuadNodeXY(float c1Min, float c2Min, float c1Max, float c2Max, QUAD_PLANE plane, float planeDirection, PlanetProperties* planet)
{
	this->depth = 0;
	type = QUAD_CHILD::ROOT;
	this->plane = plane;
	this->planeDirection = planeDirection;
	this->planet = planet;

	float z = planeDirection * planet->radius;
	
	corners.p0 = std::shared_ptr<Point>(new Point(c1Min, c2Max, z, planet->vertexBufferIndex++));
	corners.p1 = std::shared_ptr<Point>(new Point(c1Min, c2Min, z, planet->vertexBufferIndex++));
	corners.p2 = std::shared_ptr<Point>(new Point(c1Max, c2Max, z, planet->vertexBufferIndex++));
	corners.p3 = std::shared_ptr<Point>(new Point(c1Max, c2Min, z, planet->vertexBufferIndex++));

	planet->vertices.push_back(corners.p0->coords);
	planet->vertices.push_back(corners.p1->coords);
	planet->vertices.push_back(corners.p2->coords);
	planet->vertices.push_back(corners.p3->coords);
}

SurfaceQuadNodeXY::SurfaceQuadNodeXY(SurfaceQuadNode* parent, QUAD_CHILD type, float planeDirection, PlanetProperties* planet)
{
	this->parent = parent;
	this->type = type;
	this->plane = QUAD_PLANE::XY;
	this->planeDirection = parent->planeDirection;
	this->planet = planet;
	depth = parent->depth + 1;

	setNeighbours();
	initVertices();
}

void SurfaceQuadNodeXY::setNeighbours()
{
	switch (this->type)
	{
	case QUAD_CHILD::TOP_LEFT:
		if (parent->neighbourLeft != NULL)
			neighbourLeft = parent->neighbourLeft->topRight;

		if (parent->neighbourTop != NULL)
			neighbourTop = parent->neighbourTop->botLeft;
		break;
	case QUAD_CHILD::BOT_LEFT:
		if (parent->neighbourLeft != NULL)
			neighbourLeft = parent->neighbourLeft->botRight;

		if (parent->neighbourBot != NULL)
			neighbourBot = parent->neighbourBot->topLeft;

		neighbourTop = parent->topLeft;
		parent->topLeft->neighbourBot = this;
		break;
	case QUAD_CHILD::TOP_RIGHT:
		if (parent->neighbourTop != NULL)
			neighbourTop = parent->neighbourTop->botRight;

		if (parent->neighbourRight != NULL)
			neighbourRight = parent->neighbourRight->topLeft;

		neighbourLeft = parent->topLeft;
		parent->topLeft->neighbourRight = this;
		break;
	case QUAD_CHILD::BOT_RIGHT:
		if (parent->neighbourTop != NULL)
			neighbourTop = parent->neighbourTop->botRight;

		if (parent->neighbourRight != NULL)
			neighbourRight = parent->neighbourRight->topLeft;

		neighbourLeft = parent->botLeft;
		neighbourTop = parent->topRight;

		parent->botLeft->neighbourRight = this;
		parent->topRight->neighbourBot = this;
		break;
	default:
		break;
	}
}

void SurfaceQuadNodeXY::initVertices()
{
	const float x1 = parent->corners.p0->coords.x;
	const float x2 = parent->corners.p3->coords.x;
	const float y1 = parent->corners.p0->coords.y;
	const float y2 = parent->corners.p3->coords.y;

	switch (this->type)
	{
	case QUAD_CHILD::TOP_LEFT:
		corners.p0 = parent->corners.p0;

		if (neighbourLeft != nullptr)
		{
			corners.p1 = neighbourLeft->corners.p3;
		}
		else
		{
			corners.p1 = std::shared_ptr<Point>(new Point(x1, y1 - (y1 - y2) / 2, planeDirection*planet->radius, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p1->coords);
		}

		if (neighbourTop != nullptr)
		{
			corners.p2 = neighbourTop->corners.p3;
		}
		else
		{
			corners.p2 = std::shared_ptr<Point>(new Point(x1 + (x2 - x1) / 2, y1, planeDirection * planet->radius, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p2->coords);
		}

		corners.p3 = std::shared_ptr<Point>(new Point(x1 + (x2 - x1) / 2, y1 - (y1 - y2) / 2, planeDirection * planet->radius, planet->vertexBufferIndex++));
		planet->vertices.push_back(corners.p3->coords);
		break;
	case QUAD_CHILD::BOT_LEFT:
		corners.p0 = neighbourTop->corners.p1;
		corners.p1 = parent->corners.p1;
		corners.p2 = neighbourTop->corners.p3;

		if (neighbourBot != nullptr)
		{
			corners.p3 = neighbourBot->corners.p2;
		}
		else
		{
			corners.p3 = std::shared_ptr<Point>(new Point(x1 + (x2 - x1) / 2, y2, planeDirection * planet->radius, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p3->coords);
		}
		break;
	case QUAD_CHILD::TOP_RIGHT:
		corners.p0 = neighbourLeft->corners.p2;
		corners.p1 = neighbourLeft->corners.p3;
		corners.p2 = parent->corners.p2;

		if (neighbourRight != nullptr)
		{
			corners.p3 = neighbourRight->corners.p1;
		}
		else
		{
			corners.p3 = std::shared_ptr<Point>(new Point(x2, y1 - (y1 - y2) / 2, planeDirection * planet->radius, planet->vertexBufferIndex++));
			planet->vertices.push_back(corners.p3->coords);
		}
		break;
	case QUAD_CHILD::BOT_RIGHT:
		corners.p0 = neighbourLeft->corners.p2;
		corners.p1 = neighbourLeft->corners.p3;
		corners.p2 = neighbourTop->corners.p3;
		corners.p3 = parent->corners.p3;
		break;
	}
}

void SurfaceQuadNodeXY::split()
{
	if(topLeft == NULL)
	{
		topLeft = new SurfaceQuadNodeXY(this, QUAD_CHILD::TOP_LEFT, planeDirection, planet);
		botLeft = new SurfaceQuadNodeXY(this, QUAD_CHILD::BOT_LEFT, planeDirection, planet);
		topRight = new SurfaceQuadNodeXY(this, QUAD_CHILD::TOP_RIGHT, planeDirection, planet);
		botRight = new SurfaceQuadNodeXY(this, QUAD_CHILD::BOT_RIGHT, planeDirection, planet);

		return;
	}

	glm::vec3 c;
	if(corners.p0->coords.x < 0)
	{
		if(corners.p0->coords.y > 0)
		{
			c = corners.p3->coords;
		}
		else
		{
			c = corners.p2->coords;
		}
	}
	else
	{
		if (corners.p0->coords.y > 0)
		{
			c = corners.p1->coords;
		}
		else
		{
			c = corners.p0->coords;
		}
	}
	
	const float d = sqrt((c.x - 0.0f) * (c.x - 0.0f) + (c.y - 0.0f) * (c.y - 0.0f) + (c.z - planet->radius - 1)* (c.z - planet->radius - 1));

	//float targetLvl = 1.0 / ((d / (2000.0 + 3*d)) + 1.0 / (planet->maxDepth - 3.6)) + 4.0;
	float targetLvl = log2(10.0f / d * PI * planet->radius);
	if (static_cast<int>(depth) < static_cast<int>(targetLvl))
	{
		topLeft->split();
		botLeft->split();
		topRight->split();
		botRight->split();
	}
	else if (depth < 4)
	{
		topLeft->split();
		botLeft->split();
		topRight->split();
		botRight->split();
	}
}