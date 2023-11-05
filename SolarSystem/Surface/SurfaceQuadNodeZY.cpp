#include "SurfaceQuadNodeZY.h"

SurfaceQuadNodeZY::SurfaceQuadNodeZY(float c1Min, float c2Min, float c1Max, float c2Max, QUAD_PLANE plane, float planeDirection, PlanetProperties* planet)
{
	type = QUAD_CHILD::ROOT;
	this->plane = plane;
	this->planeDirection = planeDirection;
	this->planet = planet;
}

SurfaceQuadNodeZY::SurfaceQuadNodeZY(SurfaceQuadNode* parent, QUAD_CHILD type, float planeDirection, PlanetProperties* planet)
{
	this->parent = parent;
	this->type = type;
	this->plane = QUAD_PLANE::YZ;
	this->planeDirection = parent->planeDirection;
	this->planet = planet;
	depth = parent->depth + 1;

	setNeighbours();
	initVertices();
}

void SurfaceQuadNodeZY::updateTree()
{
	
}

void SurfaceQuadNodeZY::split()
{
	glm::vec3 c = glm::vec3(corners.p0->coords.x, (corners.p0->coords.y + corners.p3->coords.y) / 2, (corners.p0->coords.z + corners.p3->coords.z) / 2);
	float k = planet->radius / sqrt(c.x * c.x + c.y * c.y + c.z * c.z);
	c *= k;
	c += planet->position;
	const float d = Point::distance(c, planet->cameraP);

	float targetLvl = log2(10.0f / d * PI * planet->radius);
	if ((static_cast<int>(depth) < static_cast<int>(targetLvl) || (depth < 3)) && (depth < planet->maxDepth))
	{
		if (topLeft == NULL)
		{
			topLeft = new SurfaceQuadNodeZY(this, QUAD_CHILD::TOP_LEFT, planeDirection, planet);
			botLeft = new SurfaceQuadNodeZY(this, QUAD_CHILD::BOT_LEFT, planeDirection, planet);
			topRight = new SurfaceQuadNodeZY(this, QUAD_CHILD::TOP_RIGHT, planeDirection, planet);
			botRight = new SurfaceQuadNodeZY(this, QUAD_CHILD::BOT_RIGHT, planeDirection, planet);
		}

		topLeft->split();
		botLeft->split();
		topRight->split();
		botRight->split();
	}
	else
	{
		planet->primitives.push_back(corners.p0->index);
		planet->primitives.push_back(corners.p1->index);
		planet->primitives.push_back(corners.p2->index);
		planet->primitives.push_back(corners.p1->index);
		planet->primitives.push_back(corners.p3->index);
		planet->primitives.push_back(corners.p2->index);
	}
}

//void SurfaceQuadNodeZY::setNeighbours()
//{
//	switch (this->type)
//	{
//	case QUAD_CHILD::TOP_LEFT:
//		if (parent->neighbourLeft != NULL)
//			neighbourLeft = parent->neighbourLeft->topRight;
//
//		if (parent->neighbourTop != NULL)
//			neighbourTop = parent->neighbourTop->botLeft;
//		break;
//	case QUAD_CHILD::BOT_LEFT:
//		if (parent->neighbourLeft != NULL)
//			neighbourLeft = parent->neighbourLeft->botRight;
//
//		if (parent->neighbourBot != NULL)
//			neighbourBot = parent->neighbourBot->topLeft;
//
//		neighbourTop = parent->topLeft;
//		parent->topLeft->neighbourBot = this;
//		break;
//	case QUAD_CHILD::TOP_RIGHT:
//		if (parent->neighbourTop != NULL)
//			neighbourTop = parent->neighbourTop->botRight;
//
//		if (parent->neighbourRight != NULL)
//			neighbourRight = parent->neighbourRight->topLeft;
//
//		neighbourLeft = parent->topLeft;
//		parent->topLeft->neighbourRight = this;
//		break;
//	case QUAD_CHILD::BOT_RIGHT:
//		if (parent->neighbourRight != NULL)
//			neighbourRight = parent->neighbourRight->botLeft;
//
//		if (parent->neighbourBot != NULL)
//			neighbourBot = parent->neighbourBot->topRight;
//
//		neighbourLeft = parent->botLeft;
//		neighbourTop = parent->topRight;
//
//		parent->botLeft->neighbourRight = this;
//		parent->topRight->neighbourBot = this;
//		break;
//	default:
//		break;
//	}
//}

//void SurfaceQuadNodeZY::initVertices()
//{
//	float z1 = parent->corners.p0->coords.z;
//	float z2 = parent->corners.p3->coords.z;
//	float y1 = parent->corners.p0->coords.y;
//	float y2 = parent->corners.p3->coords.y;
//
//	switch (this->type)
//	{
//	case QUAD_CHILD::TOP_LEFT:
//		corners.p0 = parent->corners.p0;
//
//		if (neighbourLeft != NULL)
//		{
//			corners.p1 = neighbourLeft->corners.p3;
//		}
//		else
//		{
//			corners.p1 = std::shared_ptr<Point>(new Point(planeDirection * planet->radius, y1 - (y1 - y2) / 2, z1, planet->vertexBufferIndex++));
//			planet->vertices.push_back(corners.p1->coords);
//		}
//
//		if (neighbourTop != NULL)
//		{
//			corners.p2 = neighbourTop->corners.p3;
//		}
//		else
//		{
//			corners.p2 = std::shared_ptr<Point>(new Point(planeDirection * planet->radius, y1, z1 + (z2 - z1) / 2, planet->vertexBufferIndex++));
//			planet->vertices.push_back(corners.p2->coords);
//		}
//
//		corners.p3 = std::shared_ptr<Point>(new Point(planeDirection * planet->radius, y1 - (y1 - y2) / 2, z1 + (z2 - z1) / 2, planet->vertexBufferIndex++));
//		planet->vertices.push_back(corners.p3->coords);
//		break;
//	case QUAD_CHILD::BOT_LEFT:
//		corners.p0 = neighbourTop->corners.p1;
//		corners.p1 = parent->corners.p1;
//		corners.p2 = neighbourTop->corners.p3;
//
//		if (neighbourBot != NULL)
//		{
//			corners.p3 = neighbourBot->corners.p2;
//		}
//		else
//		{
//			corners.p3 = std::shared_ptr<Point>(new Point(planeDirection * planet->radius, y2, z1 + (z2 - z1) / 2, planet->vertexBufferIndex++));
//			planet->vertices.push_back(corners.p3->coords);
//		}
//		break;
//	case QUAD_CHILD::TOP_RIGHT:
//		corners.p0 = neighbourLeft->corners.p2;
//		corners.p1 = neighbourLeft->corners.p3;
//		corners.p2 = parent->corners.p2;
//
//		if (neighbourRight != NULL)
//		{
//			corners.p3 = neighbourRight->corners.p1;
//		}
//		else
//		{
//			corners.p3 = std::shared_ptr<Point>(new Point(planeDirection * planet->radius, y1 - (y1 - y2) / 2, z2, planet->vertexBufferIndex++));
//			planet->vertices.push_back(corners.p3->coords);
//		}
//		break;
//	case QUAD_CHILD::BOT_RIGHT:
//		corners.p0 = neighbourLeft->corners.p2;
//		corners.p1 = neighbourLeft->corners.p3;
//		corners.p2 = neighbourTop->corners.p3;
//		corners.p3 = parent->corners.p3;
//		break;
//	default:
//		break;
//	}
//}