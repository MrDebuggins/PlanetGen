#pragma once

#include "RandableObj.h"
#include "SurfaceQuadTree.h"


#define PI glm::pi<float>()

class Planet : virtual public RandableObj
{
	PlanetProperties properties;

	SurfaceQuadNode* rootXYPos = nullptr;
	SurfaceQuadNode* rootXYNeg = nullptr;

	SurfaceQuadNode* rootXZPos = nullptr;
	SurfaceQuadNode* rootXZNeg = nullptr;

	SurfaceQuadNode* rootYZPos = nullptr;
	SurfaceQuadNode* rootYZNeg = nullptr;
public:
	~Planet();

	Planet();

	Planet(float radius, glm::vec3 pos);

	void prepareObject();

	void update();

	void draw();

	void setCameraPos(const glm::vec3 pos);
};