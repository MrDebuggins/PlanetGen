#pragma once
#include <iostream>

#include "SurfaceQuadNodeXY.h"
#include "SurfaceQuadNodeXZ.h"
#include "SurfaceQuadNodeZY.h"
#include "IRandable.h"


#define PI glm::pi<float>()

class Planet : virtual public IRandable
{
	Shader* planetShader = new Shader("Surface/surface.vert", "Surface/surface.frag");

	PlanetProperties* properties;

	float xMin = -1;
	float yMin = -1;
	float xMax = 1;
	float yMax = 1;

	SurfaceQuadNodeXY* rootXYPos;
	SurfaceQuadNodeXY* rootXYNeg;

	SurfaceQuadNodeXZ* rootXZPos;
	SurfaceQuadNodeXZ* rootXZNeg;

	SurfaceQuadNodeZY* rootZYPos;
	SurfaceQuadNodeZY* rootZYNeg;

public:
	Planet(float radius, glm::vec3 pos);

	~Planet() 
	{
		delete planetShader;

		SurfaceQuadNode::DestroyQuadTree(rootZYPos);
		SurfaceQuadNode::DestroyQuadTree(rootZYNeg);
		SurfaceQuadNode::DestroyQuadTree(rootXZPos);
		SurfaceQuadNode::DestroyQuadTree(rootXZNeg);
		SurfaceQuadNode::DestroyQuadTree(rootXYPos);

		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &ebo);
	}

	void buildSurfaceMesh();

	void update();

	Shader* getObjShader()
	{
		if(planetShader)
			return planetShader;
	}

	std::vector<glm::vec3>* getVertexArray() override
	{
		return &(properties->vertices);
	}

	int vertexArraySize() override
	{
		return properties->vertexArraySize[properties->currentDepth];
	}

	std::vector<unsigned int>* getPrimitivesArray() override
	{
		//float d = sqrt((cameraPos.x - position.x) * (cameraPos.x - position.x) + (cameraPos.y - position.y) * (cameraPos.y - position.y) + (cameraPos.z - position.z) * (cameraPos.z - position.z)) - properties.radius;
		////float targetDepth = 1 / ((d / 2000) + 1 / (properties.maxDepth - 3.6)) + 4;
		////d = 10000.0f;
		//float targetDepth = log2(10.0f / d * PI * properties.radius);
		//if (targetDepth < 4)
		//	targetDepth = 4;

		//if(targetDepth != properties.currentDepth)
		//{
		//	properties.currentDepth = targetDepth;
		//	properties.primitives.clear();

		//	rootXYPos->pushElemntIndexes();
		//	rootXZPos->pushElemntIndexes();
		//	rootXZNeg->pushElemntIndexes();
		//	rootZYPos->pushElemntIndexes();
		//	rootZYNeg->pushElemntIndexes();
		//}

		return &(properties->primitives);
	}

	void setCameraPos(const glm::vec3 pos) override
	{
		properties->cameraP = pos;

		//float d = sqrt((position.x - pos.x) * (position.x - pos.x) + (position.y - pos.y) * (position.y - pos.y) + (position.z - pos.z) * (position.z - pos.z));
		//properties.cameraP = glm::vec3(0, 0, d);
	}

	glm::vec3 getPosition() override
	{
		return properties->position;
	}
};