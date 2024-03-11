#include "Planet.h"

#include <iostream>


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
	properties = PlanetProperties(6371000.0f, glm::vec3(0.0f));
}

Planet::Planet(float radius, glm::vec3 pos)
{
	properties = PlanetProperties(radius, pos);
}

void Planet::prepareObject()
{
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

	rootXYPos->update();
	rootXYNeg->update();

	rootXZPos->update();
	rootXZNeg->update();

	rootYZPos->update();
	rootYZNeg->update();
}

void Planet::draw()
{
	GLint viewMat = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewMat, 1, GL_FALSE, glm::value_ptr(Renderer::getViewMatrix()));

	GLint modelMat = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelMat, 1, GL_FALSE, glm::value_ptr(glm::mat4x4(1.0f)));

	GLint projMat = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projMat, 1, GL_FALSE, glm::value_ptr(Renderer::getProjectionMatrix()));

	// camera position
	GLint cameraPosmLoc = glGetUniformLocation(shaderProgram, "cameraPos_m");
	glUniform3fv(cameraPosmLoc, 1, glm::value_ptr(properties.cameraPos_m));
	GLint cameraPosMLoc = glGetUniformLocation(shaderProgram, "cameraPos_M");
	glUniform3fv(cameraPosMLoc, 1, glm::value_ptr(1000000.0f * properties.cameraPos_M));

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

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, properties.vertices.size() * sizeof(glm::vec3), properties.vertices.data(), GL_DYNAMIC_DRAW);

	glDrawArrays(GL_PATCHES, 0, properties.vertices.size());
}

void Planet::setCameraPos(glm::vec3 pos_m, glm::vec3 pos_M)
{
	properties.cameraPos_m = pos_m;
	properties.cameraPos_M = pos_M;
}

GLuint Planet::getShaderProgram()
{
	return shaderProgram;
}
