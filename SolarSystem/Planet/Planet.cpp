#include "Planet.h"

#include <iostream>
#include <omp.h>

#include "Perlin.h"


Planet::~Planet()
{
	SurfaceQuadNode::DestroyQuadTree(rootXYPos);
	SurfaceQuadNode::DestroyQuadTree(rootXYNeg);
	SurfaceQuadNode::DestroyQuadTree(rootXZPos);
	SurfaceQuadNode::DestroyQuadTree(rootXZNeg);
	SurfaceQuadNode::DestroyQuadTree(rootYZPos);
	SurfaceQuadNode::DestroyQuadTree(rootYZNeg);
}

Planet::Planet()
{
	properties = PlanetProperties(6371000.0f, glm::vec3(0.0f), nullptr);
}

Planet::Planet(std::string name, float radius, glm::vec3 pos, Camera *camera)
{
	this->name = name;
	properties = PlanetProperties(radius, pos, camera);
}

void Planet::prepareObject()
{
	this->name = name;

	// init all 6 faces as new trees
	rootXYPos = new SurfaceQuadNode(&properties, QUAD_PLANE::XY, 1,
		glm::vec3(-properties.sim_r, properties.sim_r, properties.sim_r),
		glm::vec3(0.0, 0.0, properties.sim_r));

	rootXYNeg = new SurfaceQuadNode(&properties, QUAD_PLANE::XY, -1,
		glm::vec3(properties.sim_r, properties.sim_r, -properties.sim_r),
		glm::vec3(0.0, 0.0, -properties.sim_r));

	rootXZPos = new SurfaceQuadNode(&properties, QUAD_PLANE::XZ, 1,
		glm::vec3(-properties.sim_r, properties.sim_r, -properties.sim_r),
		glm::vec3(0.0, properties.sim_r, 0.0));

	rootXZNeg = new SurfaceQuadNode(&properties, QUAD_PLANE::XZ, -1,
		glm::vec3(properties.sim_r, -properties.sim_r, -properties.sim_r),
		glm::vec3(0.0, -properties.sim_r, 0.0));

	rootYZPos = new SurfaceQuadNode(&properties, QUAD_PLANE::YZ, 1,
		glm::vec3(properties.sim_r, properties.sim_r, properties.sim_r),
		glm::vec3(properties.sim_r, 0.0, 0.0));

	rootYZNeg = new SurfaceQuadNode(&properties, QUAD_PLANE::YZ, -1,
		glm::vec3(-properties.sim_r, properties.sim_r, -properties.sim_r),
		glm::vec3(-properties.sim_r, 0.0, 0.0));

	// gen buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	// load data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,  properties.vertices.size() * sizeof(float), properties.vertices.data(), GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	// set patch vertices count
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	// create shader program
	shaderProgram = Renderer::createShaderProgram(
		"Resources/Shaders/planet.vs",
		"Resources/Shaders/planet.frag",
		"Resources/Shaders/planet.tcs",
		"Resources/Shaders/planet.tes");
}

void Planet::update()
{
	properties.vertices.clear();
	properties.currentMaxLOD = 0;
	properties.nrPatches = 0;

	//glm::vec3 tmp0 = properties.camera->position_M;
	//glm::vec3 tmp1 = properties.camera->position_m;
	//properties.camera->position_M = glm::vec3(4.00000048f, 4.4000001f, 3.70000076f);
	//properties.camera->position_m = glm::vec3(-97579.3672f, -93891.6562f, -66706.25f);

	rootXYPos->update();
	rootXYNeg->update();

	rootXZPos->update();
	rootXZNeg->update();

	rootYZPos->update();
	rootYZNeg->update();

	//properties.camera->position_M = tmp0;
	//properties.camera->position_m = tmp1;

	properties.noiseCalculated = true;
}

void Planet::draw(glm::vec3 lightPos)
{
	GLint viewMat = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewMat, 1, GL_FALSE, glm::value_ptr(Renderer::getViewMatrix()));

	GLint modelMat = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelMat, 1, GL_FALSE, glm::value_ptr(glm::mat4x4(1.0f)));

	GLint projMat = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projMat, 1, GL_FALSE, glm::value_ptr(Renderer::getProjectionMatrix()));

	// camera position
	GLint cameraPosmLoc = glGetUniformLocation(shaderProgram, "cameraPos_m");
	glUniform3fv(cameraPosmLoc, 1, glm::value_ptr(properties.camera->position_m));
	GLint cameraPosMLoc = glGetUniformLocation(shaderProgram, "cameraPos_M");
	glUniform3fv(cameraPosMLoc, 1, glm::value_ptr(coef_M * properties.camera->position_M));

	// object position
	GLint posLoc = glGetUniformLocation(shaderProgram, "objPos");
	glUniform3fv(posLoc, 1, glm::value_ptr(properties.position));

	// radius
	GLint radiusLoc = glGetUniformLocation(shaderProgram, "radius");
	glUniform1f(radiusLoc, properties.radius);

	// minimum level of detail
	GLint lodLoc = glGetUniformLocation(shaderProgram, "currentMaxLOD");
	glUniform1ui(lodLoc, properties.currentMaxLOD);

	// noise properties
	GLint layerLoc = glGetUniformLocation(shaderProgram, "layers");
	glUniform1i(layerLoc, properties.layers);

	GLint amplLoc = glGetUniformLocation(shaderProgram, "baseAmpl");
	glUniform1f(amplLoc, properties.amplitude);
	GLint persLoc = glGetUniformLocation(shaderProgram, "pers");
	glUniform1f(persLoc, properties.persistence);

	GLint resLoc = glGetUniformLocation(shaderProgram, "baseRes");
	glUniform1f(resLoc, properties.resolution);
	GLint lacLoc = glGetUniformLocation(shaderProgram, "lac");
	glUniform1f(lacLoc, properties.lacunarity);

	GLint maxAltLoc = glGetUniformLocation(shaderProgram, "maxAlt");
	glUniform1f(maxAltLoc, properties.maxAlt);

	GLint threshLoc = glGetUniformLocation(shaderProgram, "thresh");
	glUniform1f(threshLoc, properties.threshold);

	GLint modeLoc = glGetUniformLocation(shaderProgram, "noiseMode");
	glUniform1i(modeLoc, properties.mode);

	GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, properties.vertices.size() * sizeof(glm::vec3), properties.vertices.data(), GL_DYNAMIC_DRAW);

	glDrawArrays(GL_PATCHES, 0, properties.vertices.size());
}

GLuint Planet::getShaderProgram()
{
	return shaderProgram;
}

std::string Planet::getName()
{
	return name;
}

float Planet::getRadius()
{
	return properties.radius;
}
void Planet::setRadius(float r)
{
	properties.radius = r;
	properties.noiseCalculated = false;
}

glm::vec3 Planet::getPosition()
{
	return properties.position;
}
void Planet::setPosition(float x, float y, float z)
{
	properties.position = glm::vec3(x, y, z);
}

float Planet::getLODFactor()
{
	return properties.lodFactor;
}
void Planet::setLODFactor(float f)
{
	properties.lodFactor = f;
	properties.noiseCalculated = false;
}

void Planet::calcMaxAltitude()
{
	float val = properties.amplitude;
	properties.maxAlt = 0.0f;
	for (int i = 0; i < properties.layers; ++i)
	{
		properties.maxAlt += val;
		val /= properties.persistence;
	}
	properties.maxAlt *= 1.5;

	switch (properties.mode)
	{
	case 1 || 4:
		properties.maxAlt *= ridgeTransform(1.0f);
		break;
	case 2:
		properties.maxAlt *= sineTransform(PI / 2.0f);
		break;
	}
}

int Planet::getNoiseLayers()
{
	return properties.layers;
}
void Planet::setNoiseLayers(int nr)
{
	properties.layers = nr;

	calcMaxAltitude();

	properties.noiseCalculated = false;
}

float Planet::getBaseAmplitude()
{
	return properties.amplitude;
}
void Planet::setBaseAmplitude(float val)
{
	properties.amplitude = val;

	calcMaxAltitude();

	properties.noiseCalculated = false;
}

float Planet::getPersistence()
{
	return properties.persistence;
}
void Planet::setPersistence(float val)
{
	properties.persistence = val;

	calcMaxAltitude();

	properties.noiseCalculated = false;
}

float Planet::getBaseResolution()
{
	return properties.resolution;
}
void Planet::setBaseResolution(float val)
{
	properties.resolution = val;

	properties.noiseCalculated = false;
}

float Planet::getLacunarity()
{
	return properties.lacunarity;
}
void Planet::setLacunarity(float val)
{
	properties.lacunarity = val;

	properties.noiseCalculated = false;
}

float Planet::getMaxAltitude()
{
	return properties.maxAlt;
}

float Planet::getThreshold()
{
	return properties.threshold;
}
void Planet::setThreshold(float thr)
{
	properties.threshold = thr;
	properties.noiseCalculated = false;
}

int Planet::getNoiseMode()
{
	return properties.mode;
}
void Planet::setNoiseMode(int mode)
{
	properties.mode = mode;

	calcMaxAltitude();

	properties.noiseCalculated = false;
}

unsigned int Planet::getPatchesNrProcessed()
{
	return properties.nrPatches;
}
unsigned int Planet::getPatchesNrToBeSent()
{
	return properties.vertices.size() / 4;
}
