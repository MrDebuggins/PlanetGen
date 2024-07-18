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

	Planet(std::string name, float radius, glm::vec3 pos, Camera *camera);

	void prepareObject();

	void update();

	void draw(glm::vec3 lightPos);

	void setCameraPos(glm::vec3 pos_m, glm::vec3 pos_M);

	GLuint getShaderProgram();

	std::string getName();

	float getRadius();
	void setRadius(float r);

	glm::vec3 getPosition();
	void setPosition(float x, float y, float z);

	float getLODFactor();
	void setLODFactor(float f);

	void calcMaxAltitude();

	int getNoiseLayers();
	void setNoiseLayers(int nr);

	float getBaseAmplitude();
	void setBaseAmplitude(float val);

	float getPersistence();
	void setPersistence(float val);

	float getBaseResolution();
	void setBaseResolution(float val);

	float getLacunarity();
	void setLacunarity(float val);

	float getMaxAltitude();

	float getThreshold();
	void setThreshold(float thr);

	int getNoiseMode();
	void setNoiseMode(int mode);

	unsigned int getPatchesNrProcessed();
	unsigned int getPatchesNrToBeSent();
};