#pragma once

#include "math.h"


#ifndef PI
#define PI 3.141592
#endif

static float interpolate(float a0, float a1, float w)
{
	//return (a1 - a0) * w + a0;
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

static int FK(float k)
{
	return glm::floatBitsToInt(k) ^ glm::floatBitsToInt(cos(k));
}

static float hash(float a, float b)
{
	int x = FK(a); int y = FK(b);
	return float((x * x + y) * (y * y - x) + x) / float(2.14e9);
}

static glm::vec3 hash33(glm::vec3 p3)
{
	//float seed = hash(p3.x, hash(p3.y, p3.z));
	//float h1 = hash(seed, seed);
	//float h2 = hash(h1, seed);
	//float h3 = hash(h2, seed);
	//return glm::vec3(h1, h2, h3);

	p3 = glm::fract(p3 * glm::vec3(.10310973, .10301031, .09731030));
	p3 += dot(p3, glm::vec3(p3.y, p3.x, p3.z) + glm::vec3(33.33));
	return glm::fract((glm::vec3(p3.x, p3.x, p3.y) + glm::vec3(p3.y, p3.x, p3.x)) * glm::vec3(p3.z, p3.y, p3.x));
}

static float dotGridGradient3(float ix, float iy, float iz, float x, float y, float z)
{
	glm::vec3 gradient = hash33(glm::vec3(ix, iy, iz));
	glm::vec3 dist = glm::vec3(x, y, z) - glm::vec3(ix, iy, iz);
	return glm::dot(dist, gradient);
}

static float ridgeTransform(float h)
{
	return abs(h - 0.5f) * 2.0f;
}

static float sineTransform(float h)
{
	return sin(PI * (h - 1 / 2) / 2 + 1 / 2) * 1.2;
}

static float perlinAux(float x, float y, float z, float res, float ampl)
{
	float x0 = int(res * floor(x / res));
	float x1 = x0 + res;
	float y0 = int(res * floor(y / res));
	float y1 = y0 + res;
	float z0 = int(res * floor(z / res));
	float z1 = z0 + res;

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
	value = ((value / res) * 3.0f + 1.1f);

	return value * ampl;
}

static float perlin3(float x, float y, float z, float res, float ampl, float baseRes, float baseAmpl, int mode)
{
	if (mode == 3 || mode == 4)
	{
		float tmp = perlinAux(x, y, z, baseRes * 2, baseAmpl * 24);
		x += tmp;
		y += tmp;
	}

	float x0 = int(res * floor(x / res));
	float x1 = x0 + res;
	float y0 = int(res * floor(y / res));
	float y1 = y0 + res;
	float z0 = int(res * floor(z / res));
	float z1 = z0 + res;

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
	value = ((value / res) * 3.0f + 1.1f);

	if (mode == 1 || mode == 4)
		value = ridgeTransform(value);
	if (mode == 2)
		value = sineTransform(value);

	return value * ampl;
}

static float perlin5Layers(float x, float y, float z, int layers, float ampl, float pers, float res, float lac, float maxAlt, float thresh, int mode)
{
	//return perlinAux(x, y, z, res, ampl);
	float perlin = 0;
	float bRes = res, bAmpl = ampl;
	for (int i = 0; i < layers; ++i)
	{
		perlin += perlin3(x, y, z, res, ampl, bRes, bAmpl, mode);
		ampl /= pers;
		res /= lac;
	}

	if (perlin < (maxAlt * thresh))
		return maxAlt * thresh;

	return perlin;
}