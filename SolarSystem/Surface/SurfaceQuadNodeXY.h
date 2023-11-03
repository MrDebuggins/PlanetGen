#pragma once
#include "SurfaceQuadTree.h"


class SurfaceQuadNodeXY : public SurfaceQuadNode
{
public:
	SurfaceQuadNodeXY(float c1Min, float c2Min, float c1Max, float c2Max, QUAD_PLANE plane, float planeDirection, PlanetProperties* planet);

	SurfaceQuadNodeXY(SurfaceQuadNode* parent, QUAD_CHILD type, float planeDirection, PlanetProperties* planet);

	void split();

	void setNeighbours();

	void initVertices();
};