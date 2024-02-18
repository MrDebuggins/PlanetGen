#pragma once
#include "SurfaceQuadTree.h"


class SurfaceQuadNodeZY : public SurfaceQuadNode
{
public:
	SurfaceQuadNodeZY(float c1Min, float c2Min, float c1Max, float c2Max, QUAD_PLANE plane, float planeDirection, PlanetProperties* planet);

	SurfaceQuadNodeZY(SurfaceQuadNode* parent, QUAD_CHILD type, float planeDirection, PlanetProperties* planet);

	void updateTree() override;

	void split();
};