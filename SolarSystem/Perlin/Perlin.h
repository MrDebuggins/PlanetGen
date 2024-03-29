#pragma once

#include <glm/glm.hpp>
#include "math.h"


static float interpolate(float a0, float a1, float w)
{
	//return (a1 - a0) * w + a0;
	return ((a1 - a0) * (3.0 - w * 2.0) * w * w + a0);
	//return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

static glm::vec3 hash33(glm::vec3 p3)
{
	p3 = glm::fract(p3 * glm::vec3(.10310973, .10301031, .09731030));
	p3 += dot(p3, glm::vec3(p3.y, p3.x, p3.z) + glm::vec3(33.33));
	return glm::fract((glm::vec3(p3.x, p3.x, p3.y) + glm::vec3(p3.y, p3.x, p3.x)) * glm::vec3(p3.z, p3.y, p3.x));
}

static float dotGridGradient3(int ix, int iy, int iz, float x, float y, float z)
{
	glm::vec3 gradient = hash33(glm::vec3(ix, iy, iz));
	glm::vec3 dist = glm::vec3(x, y, z) - glm::vec3(ix, iy, iz);
	return glm::dot(dist, gradient);
}

static float perlin3(float x, float y, float z, int res)
{
	int x0 = int(res * floor(x / res));
	int x1 = x0 + res;
	int y0 = int(res * floor(y / res));
	int y1 = y0 + res;
	int z0 = int(res * floor(z / res));
	int z1 = z0 + res;

	float sx = (x - float(x0)) / res;
	float sy = (y - float(y0)) / res;
	float sz = (z - float(z0)) / res;

	float n0, n1, iz0, iz1, iz2, iz3, ix0, ix1, value;

	n0 = dotGridGradient3(x0, y0, z0, x, y, z);
	n1 = dotGridGradient3(x0, y0, z1, x, y, z);
	iz0 = interpolate(n0, n1, sz);

	n0 = dotGridGradient3(x1, y0, z0, x, y, z);
	n1 = dotGridGradient3(x1, y0, z1, x, y, z);
	iz1 = interpolate(n0, n1, sz);

	n0 = dotGridGradient3(x0, y1, z0, x, y, z);
	n1 = dotGridGradient3(x0, y1, z1, x, y, z);
	iz2 = interpolate(n0, n1, sz);

	n0 = dotGridGradient3(x1, y1, z0, x, y, z);
	n1 = dotGridGradient3(x1, y1, z1, x, y, z);
	iz3 = interpolate(n0, n1, sz);

	ix0 = interpolate(iz0, iz1, sx);
	ix1 = interpolate(iz2, iz3, sx);

	value = interpolate(ix0, ix1, sy);
	return value / res * 3.0f;
}

