#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>

#include "Camera.h"


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

	// perlin noise layers
	int layers = 5;

	// base amplitude
	float amplitude = 200000.0f;

	// base resolution
	float resolution = 2000000.0f;

	// amplitude persistence
	float persistence = 2.0f;

	// resolution lacunarity
	float lacunarity = 2.0f;

	float maxAlt = 2.0f * (12500.0f + 25000.0f + 50000.0f + 100000.0f + 200000.0f);

	// threshold
	float threshold = 0.5;

	// noise mode
	int mode = 0;

	// check if noise needs to be recalculated
	bool noiseCalculated = false;

	// planet position
	glm::vec3 position = glm::vec3(0.0f);

	// camera reference
	Camera* camera = nullptr;

	PlanetProperties()
	{
		radius = 6371000.0f;
		maxLOD = log2(10 * PI * radius) - 5;
	}

	/**
	 * \brief Constructor with radius parameter, based on it maxLOD will be calculated
	 * \param radius planet radius
	 * \param pos planet position
	 * \param camera camera reference
	 */
	PlanetProperties(const float radius, glm::vec3 pos, Camera *camera)
	{
		this->radius = radius;
		maxLOD = log2(10 * PI * radius) - 3;
		position = pos;
		this->camera = camera;
	}
};