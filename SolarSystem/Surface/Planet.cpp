#include "Planet.h"


Planet::Planet(float radius)
{
	xMax *= radius;
	xMin *= radius;
	yMax *= radius;
	yMin *= radius;

	properties = PlanetProperties(radius);
	position = glm::vec3(0, 0, -radius);

	rootXYPos = new SurfaceQuadNodeXY(xMin, yMin, xMax, yMax, QUAD_PLANE::XY, 1, &properties);

	rootXZPos = new SurfaceQuadNodeXZ(xMin, yMin, xMax, yMax, QUAD_PLANE::XZ, 1, &properties);
	rootXZNeg = new SurfaceQuadNodeXZ(xMin, yMin, xMax, yMax, QUAD_PLANE::XZ, -1, &properties);

	rootZYPos = new SurfaceQuadNodeZY(xMin, yMin, xMax, yMax, QUAD_PLANE::YZ, 1, &properties);
	rootZYNeg = new SurfaceQuadNodeZY(xMin, yMin, xMax, yMax, QUAD_PLANE::YZ, -1, &properties);
}

void Planet::buildSurfaceMesh() 
{
	rootXYPos->neighbourBot = rootXZNeg;
	rootXYPos->neighbourTop = rootXZPos;
	rootXYPos->neighbourLeft = rootZYNeg;
	rootXYPos->neighbourRight = rootZYPos;

	rootXZPos->neighbourBot = rootXYPos;
	rootXZPos->neighbourLeft = rootZYNeg;
	rootXZPos->neighbourRight = rootZYPos;

	rootXZNeg->neighbourTop = rootXZPos;
	rootXZNeg->neighbourLeft = rootZYNeg;
	rootXZNeg->neighbourRight = rootZYPos;

	rootZYPos->neighbourTop = rootXZPos;
	rootZYPos->neighbourBot = rootXZNeg;
	rootZYPos->neighbourLeft = rootXYPos;

	rootZYNeg->neighbourTop = rootXZPos;
	rootZYNeg->neighbourBot = rootXZNeg;
	rootZYNeg->neighbourRight = rootXYPos;

	//
	rootXZPos->corners.p1 = rootXYPos->corners.p0;
	rootXZPos->corners.p3 = rootXYPos->corners.p2;

	rootXZNeg->corners.p0 = rootXYPos->corners.p1;
	rootXZNeg->corners.p2 = rootXYPos->corners.p3;

	rootZYPos->corners.p0 = rootXYPos->corners.p2;
	rootZYPos->corners.p1 = rootXYPos->corners.p3;
	rootZYPos->corners.p2 = rootXZPos->corners.p2;
	rootZYPos->corners.p3 = rootXZNeg->corners.p3;

	rootZYNeg->corners.p0 = rootXZPos->corners.p0;
	rootZYNeg->corners.p1 = rootXZNeg->corners.p1;
	rootZYNeg->corners.p2 = rootXYPos->corners.p0;
	rootZYNeg->corners.p3 = rootXYPos->corners.p1;

	for(int i = 0; i < properties.maxDepth; ++i)
	{
		rootXYPos->split();
		rootZYPos->split();
		rootZYNeg->split();
		rootXZNeg->split();
		rootXZPos->split();

		properties.vertexArraySize[i] = properties.vertexBufferIndex;
	}
}