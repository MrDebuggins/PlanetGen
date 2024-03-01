#pragma once

#include "RandableObj.h"
#include "SurfaceQuadTree.h"


#define PI glm::pi<float>()

class Planet
{
	// buffer identifiers
	GLuint vbo;
	GLuint vao;
	GLuint ebo;

	// shader program used by object
	GLuint shaderProgram;

	PlanetProperties properties;

	SurfaceQuadNode* rootXYPos = nullptr;
	SurfaceQuadNode* rootXYNeg = nullptr;

	SurfaceQuadNode* rootXZPos = nullptr;
	SurfaceQuadNode* rootXZNeg = nullptr;

	SurfaceQuadNode* rootYZPos = nullptr;
	SurfaceQuadNode* rootYZNeg = nullptr;
public:
	~Planet();

	Planet();

	Planet(float radius, glm::vec3 pos);

	void prepareObject();

	void update();

	void draw();

	void setCameraPos(glm::vec3 pos_m, glm::vec3 pos_M);

	GLuint getShaderProgram();
};