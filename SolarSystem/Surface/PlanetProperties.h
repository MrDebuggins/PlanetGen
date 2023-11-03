#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>


#define PI glm::pi<float>()

struct PlanetProperties
{
	unsigned int vertexBufferIndex = 0;

	std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
	std::vector<unsigned int> primitives = std::vector<unsigned int>();

	int maxDepth = 4;
	int currentDepth = 0;
	int* vertexArraySize;

	float radius = 1.0f;

	glm::vec3 cameraP;

	PlanetProperties() = default;

	PlanetProperties(const float radius)
	{
		this->radius = radius;
		cameraP = glm::vec3(0, 0, 6371001);
		maxDepth = log2(10 * PI * radius);
		vertexArraySize = new int[maxDepth];
	}

	~PlanetProperties() 
	{
		vertices.clear();
		primitives.clear();
	}
};