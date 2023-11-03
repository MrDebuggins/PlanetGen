#pragma once
#include "SurfaceQuadTree.h"


class SurfaceQuadNodeXZ : public SurfaceQuadNode
{
public:
	SurfaceQuadNodeXZ(float c1Min, float c2Min, float c1Max, float c2Max, QUAD_PLANE plane, float planeDirection, PlanetProperties* planet);

	SurfaceQuadNodeXZ(SurfaceQuadNode* parent, QUAD_CHILD type, float planeDirection, PlanetProperties* planet);

	void split();

	void setNeighbours();

	void initVertices();
};