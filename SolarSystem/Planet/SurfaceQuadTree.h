#pragma once

#include "PlanetProperties.h"

#include <glm/gtc/type_ptr.hpp>


enum class QUAD_PLANE
{
	XY,
	XZ,
	YZ
};

/**
 * \brief Node class for managing a quad tree surface
 */
class SurfaceQuadNode
{
public:
	// planet info
	PlanetProperties* planet;

	// 3D plane of this quad
	QUAD_PLANE plane;

	// plane direction (-1 or 1), used for face culling
	float planeDirection;

	// top left corner used for calculating the rest if needed
	glm::vec3 topLeftCorner;

	// quad width
	float width;

	// quad center
	glm::vec3 center;

	// noise value
	float noise = 0.0f;

	// node LOD (0 - root, face of the basic cube)
	unsigned int depth;

	// parent node
	SurfaceQuadNode* parent = nullptr;

	// children ordered in counter-clock-wise on a square
	// 0 3
	// 1 2
	// , but not the vertices, they are in triangle based counter-clock-wise order:
	// 0 2
	// 1 3
	SurfaceQuadNode* child0 = nullptr;
	SurfaceQuadNode* child1 = nullptr;
	SurfaceQuadNode* child2 = nullptr;
	SurfaceQuadNode* child3 = nullptr;

	/**
	 * \brief Constructor for root nodes
	 * \param planet planet properties
	 * \param plane quad plane
	 * \param planeDirection plane direction
	 * \param corner quad top left corner
	 * \param center quad center
	 */
	SurfaceQuadNode(PlanetProperties* planet, QUAD_PLANE plane, int planeDirection, glm::vec3 corner, glm::vec3 center);

	/**
	 * \brief Constructor used for creating child nodes
	 * \param parent 
	 * \param topLeftCorner 
	 */
	SurfaceQuadNode(SurfaceQuadNode* parent, glm::vec3 topLeftCorner);

	/**
	 * \brief Destroy subtree
	 * \param node root node for the subtree
	 */
	static void DestroyQuadTree(SurfaceQuadNode* node);

	void split();

	void update();

	void pushDoubleToFloat(glm::dvec3 p);

	void pushVertices();

	bool isInDarkSide();
};