#pragma once

#include <iostream>

#include "RandableObj.h"
#include "Planet/Planet.h"
#include "Perlin/Perlin.h"


class Scene
{
	// celestial objects, mostly planets
	std::vector<Planet*> planets = std::vector<Planet*>();

	// Randable objects
	std::vector<RandableObj*> meshes = std::vector<RandableObj*>();

	// Static sun-like light source (to be implemented)
	const glm::vec3 lightPos = glm::vec3(-1000000000, 1000000000, 1000000000);

public:
	/**
	 * \brief Default camera
	 */
	Camera camera;

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
		camera.processKeyboard(static_cast<camera_movement>(key), state);
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

	/**
	 * \brief Process mouse scroll
	 * \param direction scroll direction
	 */
	void processMouseWheel(int direction)
	{
		camera.processMouseScroll(direction);
	}

	/**
	 * \brief Update scene components
	 */
	void update(const float dTime)
	{
		// update camera
		camera.update(dTime);

		// update meshes
		for (RandableObj* obj : meshes)
		{
			obj->setCameraPos(camera.position_m + coef_M * camera.position_M);
			obj->update();
		}

		// update planets
		for(Planet* p : planets)
		{
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
			p->draw(lightPos);
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

	// TODO delete
	float testFunc()
	{
		glm::vec3 p = coef_M * camera.position_M + camera.position_m;
		glm::vec3 norm = glm::normalize(p);
		glm::vec3 sph = norm * planets[0]->getRadius();
		float noise = perlin5Layers(sph.x, sph.y, sph.z, planets[0]->getNoiseLayers(), planets[0]->getBaseAmplitude(), 
			planets[0]->getPersistence(), planets[0]->getBaseResolution(), planets[0]->getLacunarity(), 
			planets[0]->getMaxAltitude(), planets[0]->getThreshold(), planets[0]->getNoiseMode());

		return glm::length(sph + norm * noise);
	}
};
