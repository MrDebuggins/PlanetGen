#include "SurfaceQuadTree.h"
#include "Perlin.h"

#include <iostream>


SurfaceQuadNode::SurfaceQuadNode(PlanetProperties* planet, QUAD_PLANE plane, int planeDirection, glm::vec3 corner, glm::vec3 center)
{
	this->planet = planet;
	this->plane = plane;
	this->planeDirection = planeDirection;
	corners.push_back(corner);
	calcCorners();
	width = planet->sim_r * 2.0f;
	this->center = center;
	depth = 0;
}

SurfaceQuadNode::SurfaceQuadNode(SurfaceQuadNode* parent, glm::vec3 topLeftCorner)
{
	this->parent = parent;
	planet = parent->planet;
	plane = parent->plane;
	planeDirection = parent->planeDirection;
	width = parent->width / 2.0f;
	corners.push_back(topLeftCorner);
	calcCorners();
	depth = parent->depth + 1;

	switch (plane)
	{
	case QUAD_PLANE::XY:
		center = glm::vec3(topLeftCorner.x + width / 2.0f * planeDirection, topLeftCorner.y - width / 2.0f, planet->sim_r * planeDirection);
		break;
	case QUAD_PLANE::XZ:
		center = glm::vec3(topLeftCorner.x + width / 2.0f * planeDirection, planet->sim_r * planeDirection, topLeftCorner.z + width / 2.0f);
		break;
	case QUAD_PLANE::YZ:
		center = glm::vec3(planet->sim_r * planeDirection, topLeftCorner.y - width / 2.0f, topLeftCorner.z - width / 2.0f * planeDirection);
		break;
	}

	calcNoise();
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
	child0 = new SurfaceQuadNode(this, corners[0]);
	child2 = new SurfaceQuadNode(this, center);

	switch (plane)
	{
	case QUAD_PLANE::XY:
		child1 = new SurfaceQuadNode(this, glm::vec3(corners[0].x, center.y, corners[0].z));
		child3 = new SurfaceQuadNode(this, glm::vec3(center.x, corners[0].y, corners[0].z));
		break;
	case QUAD_PLANE::XZ:
		child1 = new SurfaceQuadNode(this, glm::vec3(corners[0].x, corners[0].y, center.z));
		child3 = new SurfaceQuadNode(this, glm::vec3(center.x, corners[0].y, corners[0].z));
		break;
	case QUAD_PLANE::YZ:
		child1 = new SurfaceQuadNode(this, glm::vec3(corners[0].x, center.y, corners[0].z));
		child3 = new SurfaceQuadNode(this, glm::vec3(corners[0].x, corners[0].y, center.z));
		break;
	}
}

void SurfaceQuadNode::update()
{
	if(!planet->noiseCalculated)
		calcNoise();

	glm::vec3 normalizedCenter = glm::normalize(center);
	
	glm::vec3 realCenter = normalizedCenter * planet->radius + (normalizedCenter * noise[0]);

	// true if patch is behind the horizon
	const bool behindHorizon = horizonTest(normalizedCenter * planet->radius);

	// real planet radius
	float real_W = width * (planet->radius / planet->sim_r);

	// distance to camera
	float dist_M = glm::distance(realCenter + planet->position, coef_M * planet->camera->position_M + planet->camera->position_m);

	const bool distCheck = (dist_M) < (real_W * planet->lodFactor);

	if(!behindHorizon && distCheck && depth <= planet->maxLOD || depth < (4))
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

		const bool inView = frustrumTest();
		if (!behindHorizon && inView)
			pushVertices(corners);
	}
}

void SurfaceQuadNode::calcCorners()
{
	glm::vec3 topLeftCorner = corners[0];

	switch (plane)
	{
	case QUAD_PLANE::XY:
		corners.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y - width, topLeftCorner.z));
		corners.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y, topLeftCorner.z));
		corners.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y - width, topLeftCorner.z));
		break;
	case QUAD_PLANE::XZ:
		corners.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y, topLeftCorner.z + width));
		corners.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y, topLeftCorner.z));
		corners.push_back(glm::vec3(topLeftCorner.x + width * planeDirection, topLeftCorner.y, topLeftCorner.z + width));
		break;
	case QUAD_PLANE::YZ:
		corners.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y - width, topLeftCorner.z));
		corners.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y, topLeftCorner.z - width * planeDirection));
		corners.push_back(glm::vec3(topLeftCorner.x, topLeftCorner.y - width, topLeftCorner.z - width * planeDirection));
		break;
	}
}

void SurfaceQuadNode::pushVertices(std::vector<glm::vec3> corners)
{
	if (depth > planet->currentMaxLOD)
		planet->currentMaxLOD = depth;

	//planet->vertices.push_back(glm::normalize(corners[0]) * (planet->radius + noise[1]));
	//planet->vertices.push_back(glm::normalize(corners[1]) * (planet->radius + noise[2]));
	//planet->vertices.push_back(glm::normalize(corners[2]) * (planet->radius + noise[3]));
	//planet->vertices.push_back(glm::normalize(corners[3]) * (planet->radius + noise[4]));
	planet->vertices.push_back(corners[0]);
	planet->vertices.push_back(corners[1]);
	planet->vertices.push_back(corners[2]);
	planet->vertices.push_back(corners[3]);
}

bool SurfaceQuadNode::horizonTest(glm::vec3 p)
{
	glm::vec3 a = (coef_M * planet->camera->position_M - planet->position - p) + planet->camera->position_m;
	double axb = PI - glm::acos(glm::dot(glm::normalize(a), glm::normalize(p)));
	return (axb < (PI / 2.2));
}

bool SurfaceQuadNode::frustrumTest()
{
	bool visible = false;

	// top left corner test
	glm::vec3 pNorm = glm::normalize(corners[0]);
	glm::vec3 p = pNorm * planet->radius;
	pNorm *= noise[1];
	glm::vec3 a = planet->camera->front;
	glm::vec3 b = glm::normalize((p + planet->position - coef_M * planet->camera->position_M) - planet->camera->position_m + pNorm);
	double axb = glm::acos(glm::dot(a, b));
	visible = visible || (axb < (planet->camera->getMaxFieldOfView()));
	//visible = visible || (axb < (PI / 8));

	// second corner
	pNorm = glm::normalize(corners[1]);
	p = pNorm * planet->radius;
	pNorm *= noise[2];
	b = glm::normalize((p + planet->position - coef_M * planet->camera->position_M) - planet->camera->position_m + pNorm);
	axb = glm::acos(glm::dot(a, b));
	visible = visible || (axb < (planet->camera->getMaxFieldOfView()));
	//visible = visible || (axb < (PI / 8));

	// third corner
	pNorm = glm::normalize(corners[2]);
	p = pNorm * planet->radius;
	pNorm *= noise[3];
	b = glm::normalize((p + planet->position - coef_M * planet->camera->position_M) - planet->camera->position_m + pNorm);
	axb = glm::acos(glm::dot(a, b));
	visible = visible || (axb < (planet->camera->getMaxFieldOfView()));
	//visible = visible || (axb < (PI / 8));

	// fourth corner
	pNorm = glm::normalize(corners[3]);
	p = pNorm * planet->radius;
	pNorm *= noise[4];
	b = glm::normalize((p + planet->position - coef_M * planet->camera->position_M) - planet->camera->position_m + pNorm);
	axb = glm::acos(glm::dot(a, b));
	visible = visible || (axb < (planet->camera->getMaxFieldOfView()));
	//visible = visible || (axb < (PI / 8));

	return visible;
}

void SurfaceQuadNode::calcNoise()
{
	noise[0] = 0.0f; noise[1] = 0.0f; noise[2] = 0.0f; noise[3] = 0.0f; noise[4] = 0.0f;

	glm::vec3 p = glm::normalize(center) * planet->radius;
	noise[0] = perlin5Layers(p.x, p.y, p.z, planet->layers, planet->amplitude, planet->persistence, 
		planet->resolution, planet->lacunarity, planet->maxAlt, planet->threshold, planet->mode);

	for(int j = 0; j < 4; ++j)
	{
		p = glm::normalize(corners[j]) * planet->radius;

		noise[j + 1] = perlin5Layers(p.x, p.y, p.z, planet->layers, planet->amplitude, planet->persistence,
			planet->resolution, planet->lacunarity, planet->maxAlt, planet->threshold, planet->mode);
	}
}