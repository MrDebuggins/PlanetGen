#pragma once
#include "SurfaceQuadTree.h"


class SurfaceQuadNodeXY : public SurfaceQuadNode
{
public:
	SurfaceQuadNodeXY(float c1Min, float c2Min, float c1Max, float c2Max, QUAD_PLANE plane, float planeDirection, PlanetProperties* planet);

	SurfaceQuadNodeXY(SurfaceQuadNode* parent, QUAD_CHILD type, float planeDirection, PlanetProperties* planet);

	void updateTree() override;

	void split();

	glm::vec3 getCenterCoord()
	{
		return {(corners.p2->coords.x + corners.p0->coords.x) / 2, (corners.p0->coords.y + corners.p1->coords.y) / 2, corners.p0->coords.z
		};
	}
};