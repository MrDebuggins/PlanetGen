#pragma once
#include "math.h"


struct pvec
{
	float x;
	float y;
	float z;
};

static float interpolate(float a0, float a1, float w)
{
	return ((a1 - a0) * (3.0 - w * 2.0) * w * w + a0);
	//return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

static pvec hash33(pvec p3)
{
	double i;
	p3.x = modf(p3.x * 0.1031, &i);
	p3.y = modf(p3.y * 0.1030, &i);
	p3.z = modf(p3.z * 0.0973, &i);

	float dot = p3.x * (p3.x + 33.33) + p3.y * (p3.y + 33.33) + p3.z * (p3.z + 33.33);
	p3 = { p3.x + dot, p3.y + dot, p3.z + dot };
	pvec r;
	r.x = modf((p3.x + p3.y) * p3.z, &i);
	r.y = modf((p3.x + p3.x) * p3.y, &i);
	r.z = modf((p3.x + p3.y) * p3.x, &i);
	return r;
}

static float dotGridGradient3(int ix, int iy, int iz, float x, float y, float z)
{
	pvec v; v.x = ix, v.y = iy; v.z = iz;
	pvec gradient = hash33(v);

	float dx = x - float(ix);
	float dy = y - float(iy);
	float dz = z - float(iz);

	return (dx * gradient.x + dy * gradient.y + dz * gradient.z);
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
	return value;
}

