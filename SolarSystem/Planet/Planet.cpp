#include "Planet.h"

#include <iostream>
#include <omp.h>


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
	GLint periodsLoc = glGetUniformLocation(shaderProgram, "periods");
	glUniform1fv(periodsLoc, 5, properties.periods);
	GLint amplitudesLoc = glGetUniformLocation(shaderProgram, "amps");
	glUniform1fv(amplitudesLoc, 5, properties.amplitudes);
	
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

float* Planet::getAmplitudes()
{
	return properties.amplitudes;
}
void Planet::setAmplitudes(float* values)
{
	properties.amplitudes[0] = values[0];
	properties.amplitudes[1] = values[1];
	properties.amplitudes[2] = values[2];
	properties.amplitudes[3] = values[3];
	properties.amplitudes[4] = values[4];

	properties.maxAlt = 2.0f * (values[0] + values[1] + values[2] + values[3] + values[4]);

	properties.noiseCalculated = false;
}

float* Planet::getPeriods()
{
	return properties.periods;
}
void Planet::setPeriods(float* values)
{
	properties.periods[0] = values[0];
	properties.periods[1] = values[1];
	properties.periods[2] = values[2];
	properties.periods[3] = values[3];
	properties.periods[4] = values[4];

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
	properties.noiseCalculated = false;
}


unsigned long Planet::getPatchesNrToBeSent()
{
	return properties.vertices.size() / 4;
}
