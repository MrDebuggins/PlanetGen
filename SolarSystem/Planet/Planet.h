#pragma once

#include "RandableObj.h"
#include "SurfaceQuadTree.h"


#define PI glm::pi<float>()

class Planet
{
	// planet name
	std::string name;

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

	Planet(std::string name, float radius, glm::vec3 pos);

	void prepareObject();

	void update();

	void draw();

	void setCameraPos(glm::vec3 pos_m, glm::vec3 pos_M);

	GLuint getShaderProgram();

	std::string getName();

	float getRadius();

	void setRadius(float r);

	glm::vec3 getPosition();

	void setPosition(float x, float y, float z);

	float* getAmplitudes();

	void setAmplitudes(float* values);

	float* getPeriods();

	void setPeriods(float* values);

	unsigned long getPatchesNrToBeSent();
};