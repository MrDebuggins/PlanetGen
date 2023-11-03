#include "Renderer.h"

#include <GL/freeglut_std.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>


Renderer::Renderer(Camera* camera)
{
	this->camera = camera;

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glViewport(0, 0, windowW, windowH);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 0);
	//glEnable(GL_DEPTH_BUFFER);

	glewInit();
	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::attachShader(GLuint programme, GLuint vs, GLuint fs)
{
	glAttachShader(programme, vs);
	glAttachShader(programme, fs);
	glLinkProgram(programme);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Renderer::registerObject(IRandable* obj)
{
	float tst[] = {
		-1,  1,  0,
		-1, -1,  0,
		 1, -1,  0,
		 1,  1,  0
	};

	unsigned int prim[] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0
	};


	GLuint vbo, vao, ebo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, obj->getVertexArray()->size() * sizeof(glm::vec3), obj->getVertexArray()->data(), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), tst, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->getPrimitivesArray()->size() * sizeof(unsigned int), obj->getPrimitivesArray()->data(), GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8 * sizeof(unsigned int), prim, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	obj->setVbo(vbo);
	obj->setVao(vao);
	obj->setEbo(ebo);
}

void Renderer::setLightPos(const glm::vec3 light_pos)
{
	this->lightPos = light_pos;
}

void Renderer::draw(IRandable* obj)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint programme;
	if(obj->getObjShader())
	{
		programme = obj->getObjShader()->getShaderProgramme();
	}
	else
	{
		programme = defaultShader.getShaderProgramme();
		
	}
	
	glUseProgram(programme);

	glBindVertexArray(obj->getVbo());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->getEbo());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->getPrimitivesArray()->size() * sizeof(unsigned int), obj->getPrimitivesArray()->data(), GL_STATIC_DRAW);

	//GLuint lightPosLoc = glGetUniformLocation(programme, "lightPos");
	//glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	// camera position for translating, as camera should be in system origin
	GLuint camerPosLoc = glGetUniformLocation(programme, "cameraPos");
	glUniform3fv(camerPosLoc, 1, glm::value_ptr(camera->position));

	modelMatrix *= glm::translate(obj->getPosition());

	GLuint modelMatrixLoc = glGetUniformLocation(programme, "modelViewProjectionMatrix");
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix * camera->getViewMatrix() * modelMatrix));

	GLuint rot = glGetUniformLocation(programme, "rot");
	glUniformMatrix4fv(rot, 1, GL_FALSE, glm::value_ptr(glm::rotate(tmp, glm::vec3(0, 1, 0))));
	tmp += 1.0f/63710000.0f;

	glDrawElements(GL_TRIANGLES, obj->getPrimitivesArray()->size(), GL_UNSIGNED_INT, 0);

	modelMatrix = glm::mat4(); // matricea de modelare este matricea identitate

	glutSwapBuffers();
}

void Renderer::reshape(const int w, const int h)
{
	glViewport(0, 0, w, h);

	projectionMatrix = glm::perspective(PI / 4, static_cast<float>(w) / static_cast<float>(h), 0.1f, 10000000000.0f);

	camera->w = w;
	camera->h = h;
}