#include "SurfaceQuadTree.h"


SurfaceQuadNode::SurfaceQuadNode(PlanetProperties* planet, QUAD_PLANE plane, int planeDirection, glm::vec3 corner, glm::vec3 center)
{
	this->planet = planet;
	this->plane = plane;
	this->planeDirection = planeDirection;
	topLeftCorner = corner;
	width = planet->radius * 2.0f;
	this->center = center;
	depth = 0;

	pushVertices();
}

SurfaceQuadNode::SurfaceQuadNode(SurfaceQuadNode* parent, glm::vec3 topLeftCorner)
{
	this->parent = parent;
	planet = parent->planet;
	plane = parent->plane;
	planeDirection = parent->planeDirection;
	width = parent->width / 2.0f;
	this->topLeftCorner = topLeftCorner;
	depth = parent->depth + 1;

	switch (plane)
	{
	case QUAD_PLANE::XY:
		center = glm::vec3(topLeftCorner.x + width / 2.0f * planeDirection, topLeftCorner.y - width / 2.0f, planet->radius * planeDirection);
		break;
	case QUAD_PLANE::XZ:
		center = glm::vec3(topLeftCorner.x + width / 2.0f * planeDirection, planet->radius * planeDirection, topLeftCorner.z + width / 2.0f);
		break;
	case QUAD_PLANE::YZ:
		center = glm::vec3(planet->radius * planeDirection, topLeftCorner.y - width / 2.0f, topLeftCorner.z - width / 2.0f * planeDirection);
		break;
	}
}

void SurfaceQuadNode::DestroyQuadTree(SurfaceQuadNode* node)
{
	if (node)
	{
		DestroyQuadTree(node->child0);
		DestroyQuadTree(node->child1);
		DestroyQuadTree(node->child2);
		DestroyQuadTree(node->child3);

		node->planet = nullptr;
		node->parent = nullptr;

		node->child0 = nullptr;
		node->child1 = nullptr;
		node->child2 = nullptr;
		node->child3 = nullptr;

		delete node;
	}
}

void SurfaceQuadNode::split()
{
	child0 = new SurfaceQuadNode(this, topLeftCorner);
	child2 = new SurfaceQuadNode(this, center);

	switch (plane)
	{
	case QUAD_PLANE::XY:
		child1 = new SurfaceQuadNode(this, glm::vec3(topLeftCorner.x, center.y, topLeftCorner.z));
		child3 = new SurfaceQuadNode(this, glm::vec3(center.x, topLeftCorner.y, topLeftCorner.z));
		break;
	case QUAD_PLANE::XZ:
		child1 = new SurfaceQuadNode(this, glm::vec3(topLeftCorner.x, topLeftCorner.y, center.z));
		child3 = new SurfaceQuadNode(this, glm::vec3(center.x, topLeftCorner.y, topLeftCorner.z));
		break;
	case QUAD_PLANE::YZ:
		child1 = new SurfaceQuadNode(this, glm::vec3(topLeftCorner.x, center.y, topLeftCorner.z));
		child3 = new SurfaceQuadNode(this, glm::vec3(topLeftCorner.x, topLeftCorner.y, center.z));
		break;
	}
}

void SurfaceQuadNode::update()
{

	const bool darkSide = isInDarkSide();

	// offset point with planet position and project it to a sphere
	glm::vec3 worldSystemSphericalCenter = glm::normalize(center + planet->position) * planet->radius;
	const bool distCheck = glm::distance(worldSystemSphericalCenter, planet->cameraPos) < (width * planet->lodFactor);

	if(!darkSide && distCheck && depth < planet->maxLOD)
	{
		if (child0 == nullptr)
		{
			split();
		}

		child0->update();
		child1->update();
		child2->update();
		child3->update();
	}
	else
	{
		if (child0 != nullptr)
		{
			DestroyQuadTree(child0);
			child0 = nullptr;

			DestroyQuadTree(child1);
			child1 = nullptr;

			DestroyQuadTree(child2);
			child2 = nullptr;

			DestroyQuadTree(child3);
			child3 = nullptr;
		}

		pushVertices();
	}
}

void SurfaceQuadNode::pushVertices()
{
	planet->vertices.push_back(topLeftCorner);

	switch (plane)
	{
	case QUAD_PLANE::XY:
		planet->vertices.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y - width, topLeftCorner.z));
		planet->vertices.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y, topLeftCorner.z));
		planet->vertices.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y - width, topLeftCorner.z));
		break;
	case QUAD_PLANE::XZ:
		planet->vertices.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y, topLeftCorner.z + width));
		planet->vertices.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y, topLeftCorner.z));
		planet->vertices.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y, topLeftCorner.z + width));
		break;
	case QUAD_PLANE::YZ:
		planet->vertices.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y - width, topLeftCorner.z));
		planet->vertices.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y, topLeftCorner.z - width * planeDirection));
		planet->vertices.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y - width, topLeftCorner.z - width * planeDirection));
		break;
	}
}

bool SurfaceQuadNode::isInDarkSide()
{
	glm::vec3 cam = planet->cameraPos - planet->position;
	center -= planet->position;

	double axb = cam.x * center.x + cam.y * center.y + cam.z * center.z;

	return (axb < 0);
}

