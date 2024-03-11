#include "SurfaceQuadTree.h"
#include "Perlin.h"

#include <iostream>

const float coef_M = 1000000.0f;

SurfaceQuadNode::SurfaceQuadNode(PlanetProperties* planet, QUAD_PLANE plane, int planeDirection, glm::vec3 corner, glm::vec3 center)
{
	this->planet = planet;
	this->plane = plane;
	this->planeDirection = planeDirection;
	topLeftCorner = corner;
	width = planet->sim_r * 2.0f;
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
		center = glm::dvec3(topLeftCorner.x + width / 2.0f * planeDirection, topLeftCorner.y - width / 2.0f, planet->sim_r * planeDirection);
		break;
	case QUAD_PLANE::XZ:
		center = glm::dvec3(topLeftCorner.x + width / 2.0f * planeDirection, planet->sim_r * planeDirection, topLeftCorner.z + width / 2.0f);
		break;
	case QUAD_PLANE::YZ:
		center = glm::dvec3(planet->sim_r * planeDirection, topLeftCorner.y - width / 2.0f, topLeftCorner.z - width / 2.0f * planeDirection);
		break;
	}

	glm::vec3 realCenter = glm::normalize(center) * planet->radius + planet->position;
	noise = planet->amplitudes[0] * perlin3(realCenter.x, realCenter.y, realCenter.z, planet->periods[0]);
	noise += planet->amplitudes[1] * perlin3(realCenter.x, realCenter.y, realCenter.z, planet->periods[1]);
	noise += planet->amplitudes[2] * perlin3(realCenter.x, realCenter.y, realCenter.z, planet->periods[2]);
	noise += planet->amplitudes[3] * perlin3(realCenter.x, realCenter.y, realCenter.z, planet->periods[3]);
	noise += planet->amplitudes[4] * perlin3(realCenter.x, realCenter.y, realCenter.z, planet->periods[4]);
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
		child1 = new SurfaceQuadNode(this, glm::dvec3(topLeftCorner.x, center.y, topLeftCorner.z));
		child3 = new SurfaceQuadNode(this, glm::dvec3(center.x, topLeftCorner.y, topLeftCorner.z));
		break;
	case QUAD_PLANE::XZ:
		child1 = new SurfaceQuadNode(this, glm::dvec3(topLeftCorner.x, topLeftCorner.y, center.z));
		child3 = new SurfaceQuadNode(this, glm::dvec3(center.x, topLeftCorner.y, topLeftCorner.z));
		break;
	case QUAD_PLANE::YZ:
		child1 = new SurfaceQuadNode(this, glm::dvec3(topLeftCorner.x, center.y, topLeftCorner.z));
		child3 = new SurfaceQuadNode(this, glm::dvec3(topLeftCorner.x, topLeftCorner.y, center.z));
		break;
	}
}

void SurfaceQuadNode::update()
{
	if (depth > planet->currentMaxLOD)
		planet->currentMaxLOD = depth;

	// true if patch is in dark side of the planet
	const bool darkSide = isInDarkSide();

	// real planet radius
	float real_W = width * (planet->radius / planet->sim_r);

	// distance to camera
	float dist = glm::distance(glm::normalize(center) * (planet->radius + noise) + planet->position, coef_M * planet->cameraPos_M + planet->cameraPos_m);

	const bool distCheck = (dist) < (real_W * planet->lodFactor);

	if(!darkSide && distCheck && depth <= planet->maxLOD || depth < 3)
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

void SurfaceQuadNode::pushDoubleToFloat(glm::dvec3 p)
{
	double pxH, pxL, pyH, pyL, pzH, pzL;

	pxL = std::modf(p.x, &pxH);
	pyL = std::modf(p.y, &pyH);
	pzL = std::modf(p.z, &pzH);

	planet->vertices.push_back(glm::vec3(pxH, pyH, pzH));
	planet->vertices.push_back(glm::vec3(pxL, pyL, pzL));
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
	glm::dvec3 cam = (1000000.0f * planet->cameraPos_M - planet->position) + planet->cameraPos_m;
	glm::vec3 worldCoordsCenter = center - planet->position;

	double axb = cam.x * worldCoordsCenter.x + cam.y * worldCoordsCenter.y + cam.z * worldCoordsCenter.z;

	return (axb < 0);
}

