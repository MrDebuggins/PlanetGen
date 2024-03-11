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
	float lodFactor = 2.0f;

	// vertices from all planes
	std::vector<glm::vec3> vertices = std::vector<glm::vec3>();

	// planet radius
	float radius = 1.0f;

	// radius used for generation (a power of 2 is better for precision)
	float sim_r = 256.0f;

	// maximum level of detail, dependent of planet radius, ex: Earth radius = 6371000 m, maxLOD = 27 - 4
	// last 4 levels will be complemented with tessellation
	unsigned int maxLOD;

	// maximum level of detail for current camera position
	unsigned int currentMaxLOD = 0;

	// periods (aka. resolution, inverse of frequency) 
	float periods[5] = { 1, 10, 100, 10000, 5000000 };

	// amplitudes
	float amplitudes[5] = { 0.5f, 5.0f, 50.0f, 1000.0f, 10000.0f };

	// planet position
	glm::vec3 position = glm::vec3(0.0f);

	// camera position in meters
	glm::vec3 cameraPos_m = glm::vec3(0.0f);

	// camera position in mega meters
	glm::vec3 cameraPos_M = glm::vec3(0.0f);

	PlanetProperties()
	{
		radius = 6371000.0f;
		maxLOD = log2(10 * PI * radius) - 5;
	}

	/**
	 * \brief Constructor with radius parameter, based on it maxLOD will be calculated
	 * \param radius planet radius
	 */
	PlanetProperties(const float radius, glm::vec3 pos)
	{
		this->radius = radius;
		maxLOD = log2(10 * PI * radius) - 5;

		position = pos;
	}
};