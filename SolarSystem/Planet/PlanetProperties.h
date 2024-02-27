#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>


#ifndef PI
#define PI glm::pi<float>()
#endif


/**
 * \brief Class to store planet properties. Is passed as pointer to surface nodes on creation
 */
struct PlanetProperties
{
	// level of detail factor, greater means better detail
	float lodFactor = 1.0f;

	// vertices from all planes
	std::vector<glm::vec3> vertices = std::vector<glm::vec3>();

	// planet radius
	float radius = 1.0f;

	// radius used for generation (a power of 2 is better for precision)
	float sim_r = 256.0f;

	// maximum level of detail, dependent of planet radius, ex: Earth radius = 6371000 m, maxLOD = 27 - 6
	// last 6 levels will be complemented with tessellation
	unsigned int maxLOD;

	// planet position
	glm::vec3 position;

	// camera position
	glm::vec3 cameraPos;

	PlanetProperties()
	{
		radius = 6371000.0f;
		maxLOD = log2(10 * PI * radius) - 6;
	}

	/**
	 * \brief Constructor with radius parameter, based on it maxLOD will be calculated
	 * \param radius planet radius
	 */
	PlanetProperties(const float radius, glm::vec3 pos, glm::vec3 camPos)
	{
		this->radius = radius;
		maxLOD = log2(10 * PI * radius) - 6;

		position = pos;
		cameraPos = camPos;
	}
};