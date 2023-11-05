#include "Planet.h"


Planet::Planet(float radius, glm::vec3 pos)
{
	xMax *= radius;
	xMin *= radius;
	yMax *= radius;
	yMin *= radius;

	properties = new PlanetProperties(radius, pos);
	position = pos;

	rootXYPos = new SurfaceQuadNodeXY(xMin, yMin, xMax, yMax, QUAD_PLANE::XY, 1, properties);
	rootXYNeg = new SurfaceQuadNodeXY(xMin, yMin, xMax, yMax, QUAD_PLANE::XY, -1, properties);

	rootXZPos = new SurfaceQuadNodeXZ(xMin, yMin, xMax, yMax, QUAD_PLANE::XZ, 1, properties);
	rootXZNeg = new SurfaceQuadNodeXZ(xMin, yMin, xMax, yMax, QUAD_PLANE::XZ, -1, properties);

	rootZYPos = new SurfaceQuadNodeZY(xMin, yMin, xMax, yMax, QUAD_PLANE::YZ, 1, properties);
	rootZYNeg = new SurfaceQuadNodeZY(xMin, yMin, xMax, yMax, QUAD_PLANE::YZ, -1, properties);

	/*buildSurfaceMesh();*/
	rootXYPos->split();
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
	rootXZPos->neighbourTop = rootXYNeg;

	rootXZNeg->neighbourTop = rootXYPos;
	rootXZNeg->neighbourLeft = rootZYNeg;
	rootXZNeg->neighbourRight = rootZYPos;
	rootXZNeg->neighbourBot = rootXYNeg;

	rootZYPos->neighbourTop = rootXZPos;
	rootZYPos->neighbourBot = rootXZNeg;
	rootZYPos->neighbourLeft = rootXYPos;
	rootZYPos->neighbourRight = rootXYNeg;

	rootZYNeg->neighbourTop = rootXZPos;
	rootZYNeg->neighbourBot = rootXZNeg;
	rootZYNeg->neighbourRight = rootXYPos;
	rootZYNeg->neighbourLeft = rootXYNeg;

	rootXYNeg->neighbourLeft = rootZYPos;
	rootXYNeg->neighbourRight = rootZYNeg;
	rootXYNeg->neighbourTop = rootXZPos;
	rootXYNeg->neighbourBot = rootXZNeg;

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

	rootXYNeg->corners.p0 = rootXZPos->corners.p2;
	rootXYNeg->corners.p1 = rootXZNeg->corners.p3;
	rootXYNeg->corners.p2 = rootXZPos->corners.p0;
	rootXYNeg->corners.p3 = rootXZNeg->corners.p1;
}

void Planet::update()
{
	properties->vertices.clear();
	properties->primitives.clear();
	properties->vertexBufferIndex = 0;
	SurfaceQuadNode::DestroyQuadTree(rootXYPos);
	SurfaceQuadNode::DestroyQuadTree(rootXYNeg);
	SurfaceQuadNode::DestroyQuadTree(rootZYPos);
	SurfaceQuadNode::DestroyQuadTree(rootZYNeg);
	SurfaceQuadNode::DestroyQuadTree(rootXZPos);
	SurfaceQuadNode::DestroyQuadTree(rootXZNeg);

	rootXYPos = new SurfaceQuadNodeXY(xMin, yMin, xMax, yMax, QUAD_PLANE::XY, 1, properties);
	rootXZPos = new SurfaceQuadNodeXZ(xMin, yMin, xMax, yMax, QUAD_PLANE::XZ, 1, properties);
	rootXZNeg = new SurfaceQuadNodeXZ(xMin, yMin, xMax, yMax, QUAD_PLANE::XZ, -1, properties);
	rootZYPos = new SurfaceQuadNodeZY(xMin, yMin, xMax, yMax, QUAD_PLANE::YZ, 1, properties);
	rootZYNeg = new SurfaceQuadNodeZY(xMin, yMin, xMax, yMax, QUAD_PLANE::YZ, -1, properties);
	rootXYNeg = new SurfaceQuadNodeXY(xMin, yMin, xMax, yMax, QUAD_PLANE::XY, -1, properties);

	buildSurfaceMesh();

	rootXYPos->split();
	rootXYNeg->split();
	rootXZPos->split();
	rootXZNeg->split();
	rootZYPos->split();
	rootZYNeg->split();
}
