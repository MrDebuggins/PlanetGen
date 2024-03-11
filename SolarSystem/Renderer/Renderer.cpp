#pragma once

#include "Renderer.h"
#include "Shader.h"

#include <glm/gtx/transform.hpp>

float Renderer::windowW = d_width;
float Renderer::windowH = d_height;
glm::mat4x4 Renderer::projectionMatrix = glm::perspective(PI / 4, windowW / windowH, 0.1f, 10000000000.0f);
glm::vec3 Renderer::lightPos;
GLuint Renderer::defaultShaderProgram;
Camera* Renderer::camera;


void Renderer::init(Camera* c)
{
	camera = c;

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glViewport(0, 0, windowW, windowH);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 0);

	glewInit();

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

	// create default shadder program
	defaultShaderProgram = Shader::createShaderProgram("Resources/Shaders/default_vertex.vs", "Resources/Shaders/default_fragment.frag");
}

GLuint Renderer::createShaderProgram(const std::string& vs_file, const std::string& fs_file, const std::string& tcs_file, const std::string& tes_file, const std::string& gs_file)
{
	return Shader::createShaderProgram(vs_file, fs_file, tcs_file, tes_file, gs_file);
}

GLuint Renderer::getDefaultShaderProgram()
{
	return defaultShaderProgram;
}

void Renderer::setLightPos(const glm::vec3 light_pos)
{
	lightPos = light_pos;
}

void Renderer::reshape(const int w, const int h)
{
	windowW = w;
	windowH = h;

	glViewport(0, 0, w, h);

	projectionMatrix = glm::perspective(PI / 4, static_cast<float>(w) / static_cast<float>(h), 0.1f, 10000000000.0f);

	camera->w = w;
	camera->h = h;
}

void Renderer::useShaderProgram(GLuint shaderProgram)
{
	glUseProgram(shaderProgram);
}

glm::mat4x4 Renderer::getViewMatrix()
{
	return camera->getViewMatrix();
}

glm::mat4x4 Renderer::getProjectionMatrix()
{
	return projectionMatrix;
}

void Renderer::switchLineView(bool v)
{
	if(v)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}