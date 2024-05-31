#pragma once

#include <iostream>

#include "RandableObj.h"
#include "Planet/Planet.h"


class Scene
{
	/**
	 * \brief Default camera
	 */
	Camera camera;

	// celestial objects, mostly planets
	std::vector<Planet*> planets = std::vector<Planet*>();

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
		planets.clear();
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
		obj->setCameraPos(camera.position_m + camera.position_M);
		meshes.push_back(obj);
	}

	void addPlanet(Planet* p)
	{
		p->prepareObject();
		p->setCameraPos(camera.position_m, camera.position_M);
		planets.push_back(p);
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
		camera.processMouseMovement(x, y);
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

		// update meshes
		for (RandableObj* obj : meshes)
		{
			obj->setCameraPos(camera.position_m + 1000000.0f * camera.position_M);
			obj->update();
		}

		// update planets
		for(Planet* p : planets)
		{
			p->setCameraPos(camera.position_m, camera.position_M);
			p->update();
		}
	}

	/**
	 * \brief Draw scene components
	 */
	void draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Renderer::useShaderProgram(Renderer::getDefaultShaderProgram());
		for (RandableObj* obj : meshes)
		{
			obj->draw();
		}

		for(Planet* p : planets)
		{
			Renderer::useShaderProgram(p->getShaderProgram());
			p->draw();
		}

		glutSwapBuffers();
	}

	Planet* getPlanetById(int id)
	{
		return planets[id];
	}

	std::vector<std::string> getPlanetNames()
	{
		std::vector<std::string> names;
		for(int i = 0; i < planets.size(); ++i)
		{
			names.push_back(planets[i]->getName());
		}

		return names;
	}

	unsigned long getPatchesToBeSentToGPU()
	{
		unsigned long nr = 0;
		for(int i = 0; i < planets.size(); ++i)
		{
			nr += planets[i]->getPatchesNrToBeSent();
		}

		return nr;
	}
};
