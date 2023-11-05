#pragma once

#include "Renderer/Renderer.h"

#include <vector>
#include <GL/freeglut.h>
#include <glm/detail/type_vec3.hpp>


class Scene
{
	Camera camera;
	Renderer* renderer;

	// randable objects
	std::vector<IRandable*> meshes = std::vector<IRandable*>();

	// static sun-like light source
	const glm::vec3 lightPos = glm::vec3(-10000000.0, 10000000.0, static_cast<float>(3e11));

	// time since last input update
	float lastUpdateTime;

public:
	~Scene()
	{
		meshes.clear();

		delete renderer;
	}

	void initRenderer() 
	{
		renderer = new Renderer(&camera);
	}

	void addObject(IRandable* obj) 
	{
		obj->setCameraPos(camera.position);
		meshes.push_back(obj);

		if (obj->getObjShader())
		{
			renderer->attachShader(obj->getObjShader()->getShaderProgramme(), obj->getObjShader()->getVertexShader(), obj->getObjShader()->getFragmentShader());
		}

		renderer->registerObject(obj);
	}

	void processKeyboard(unsigned char key, int x, int y)
	{
		// update camera
		camera.processKeyboard(static_cast<camera_movement>(key), 0.3f);

		for (IRandable* obj : meshes)
		{
			obj->setCameraPos(camera.position);
			obj->update();
		}

		glutPostRedisplay();
	}

	void processMouseMovement(const int x, const int y)
	{
		camera.processMouseMovement(x, y, true);
		glutPostRedisplay();
	}

	void processMouseWheel(){}

	void draw()
	{
		for (IRandable* obj : meshes)
		{
			renderer->draw(obj);
		}
	}

	void reshape(int w, int h) { renderer->reshape(w, h); }
};
