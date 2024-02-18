#pragma once

#include "RandableObj.h"


class Scene
{
	/**
	 * \brief Default camera
	 */
	Camera camera;

	// Randable objects
	std::vector<RandableObj*> meshes = std::vector<RandableObj*>();

	// Static sun-like light source (to be implemented)
	const glm::vec3 lightPos = glm::vec3(-10000000.0, 10000000.0, static_cast<float>(3e11));

	// Time since last scene update
	float lastUpdateTime = 0;

public:
	~Scene()
	{
		meshes.clear();
	}

	/**
	 * \brief Initializes renderer with specified camera
	 */
	void initRenderer() 
	{
		Renderer::init(&camera);
	}

	/**
	 * \brief Add randable object to scene
	 * \param obj 
	 */
	void addObject(RandableObj* obj) 
	{
		// init mesh buffers
		obj->prepareObject();

		// pass camera position to object
		obj->setCameraPos(camera.position);

		meshes.push_back(obj);
	}

	/**
	 * \brief Update camera movement buttons state
	 * \param key input key
	 * \param x unused in this case, required by freeglut
	 * \param y unused in this case, required by freeglut
	 * \param state 
	 */
	void processKeyboard(unsigned char key, int x, int y, const bool state)
	{
		// update camera
		camera.processKeyboard(static_cast<camera_movement>(key), 0.3f, state);
	}

	/**
	 * \brief Update camera looking direction. Mouse is positioned in center at the end
	 * \param x horizontal offset
	 * \param y vertical offset
	 */
	void processMouseMovement(const int x, const int y)
	{
		camera.processMouseMovement(x, y, false);
	}

	// TODO probably bind to camera speed
	void processMouseWheel(){}

	/**
	 * \brief Update scene components
	 */
	void update()
	{
		// update camera
		camera.update();

		//update meshes
		for (RandableObj* obj : meshes)
		{
			obj->setCameraPos(camera.position);
			obj->update();
		}
	}

	/**
	 * \brief Draw scene components
	 */
	void draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (RandableObj* obj : meshes)
		{
			Renderer::useShaderProgram(obj->getShaderProgram());
			obj->draw();
		}

		glutSwapBuffers();
	}
};
