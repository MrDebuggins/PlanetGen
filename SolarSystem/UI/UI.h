#pragma once
#include "../Scene.h"
#include <GL/glui.h>


#define RADIUS_CTRL 1
#define POS_CTRL 2
#define LAYER_CTRL 3
#define THR_CTRL 4
#define MODE_CTRL 5
#define LOD_CTRL 6

struct PlanetData
{
	int id;
	float radius;
	float posX, posY, posZ;
	float lod;

	float thresh;
	int mode;

	int layers;
	float ampl, pers, res, lac;
};

class UI
{
	static Scene* scene;
	static GLUI* glui;
	static bool hidden;

	// controls
	static GLUI_Rollout *planetCtrl;
	static GLUI_Rollout *noiseCtrl;
	static GLUI_Rollout *layersCtrl;
	static GLUI_Listbox *planets;

	static GLUI_Spinner* planetRadius;
	static GLUI_Spinner* planetPosX;
	static GLUI_Spinner* planetPosY;
	static GLUI_Spinner* planetPosZ;
	static GLUI_Spinner* lodfactor;

	static GLUI_Spinner* layersNr;
	static GLUI_Spinner* baseAmpl;
	static GLUI_Spinner* baseRes;
	static GLUI_Spinner* persistence;
	static GLUI_Spinner* lacunarity;

	static GLUI_Spinner* threshold;
	static GLUI_Listbox* modes;

	static GLUI_StaticText* fpsCounter;
	static GLUI_StaticText* frameTime;
	static GLUI_StaticText* patches;
	static GLUI_StaticText* dist;
	static GLUI_StaticText* alt;

	//static GLUI_Spinner* factor;

	// control variables
	static int wireframe;
	static PlanetData planet;
	static int spinFactor;

	// callbacks
	static void wireframeCB(int control)
	{
		if(control == 1)
			Renderer::switchLineView();
		//else if(control == 2)
		//{
		//	planetRadius->set_speed(spinFactor);
		//	planetPosX->set_speed(spinFactor);
		//	planetPosY->set_speed(spinFactor);
		//	planetPosZ->set_speed(spinFactor);
		//	baseAmpl->set_speed(spinFactor);
		//	persistence->set_speed(spinFactor);
		//	baseRes->set_speed(spinFactor);
		//	lacunarity->set_speed(spinFactor);
		//	layer2Ampl->set_speed(spinFactor);
		//	layer2Res->set_speed(spinFactor);
		//	layer3Ampl->set_speed(spinFactor);
		//	layer3Res->set_speed(spinFactor);
		//	layer4Ampl->set_speed(spinFactor);
		//	layer4Res->set_speed(spinFactor);
		//}
	}

	static void planetSelectCB(int control)
	{
		planetRadius->set_float_val(scene->getPlanetById(planet.id)->getRadius());

		planetPosX->set_float_val(scene->getPlanetById(planet.id)->getPosition().x / spinFactor);
		planetPosY->set_float_val(scene->getPlanetById(planet.id)->getPosition().y / spinFactor);
		planetPosZ->set_float_val(scene->getPlanetById(planet.id)->getPosition().z / spinFactor);
		lodfactor->set_float_val(scene->getPlanetById(planet.id)->getLODFactor());

		modes->set_int_val(scene->getPlanetById(planet.id)->getNoiseMode());
		threshold->set_float_val(scene->getPlanetById(planet.id)->getThreshold());

		layersNr->set_int_val(scene->getPlanetById(0)->getNoiseLayers());

		baseAmpl->set_float_val(scene->getPlanetById(planet.id)->getBaseAmplitude() / spinFactor);
		persistence->set_float_val(scene->getPlanetById(planet.id)->getPersistence());

		baseRes->set_float_val(scene->getPlanetById(planet.id)->getBaseResolution() / spinFactor);
		lacunarity->set_float_val(scene->getPlanetById(planet.id)->getLacunarity());
	}

	static void planetFloats(int control)
	{
		if (control == RADIUS_CTRL)
			scene->getPlanetById(planet.id)->setRadius(planetRadius->get_float_val() * spinFactor);
		else if (control == POS_CTRL)
			scene->getPlanetById(planet.id)->setPosition(planetPosX->get_float_val() * spinFactor,
				planetPosY->get_float_val() * spinFactor, planetPosZ->get_float_val() * spinFactor);
		else if (control == LAYER_CTRL)
		{
			Planet* p = scene->getPlanetById(planet.id);

			p->setNoiseLayers(planet.layers);
			p->setBaseAmplitude(planet.ampl * spinFactor);
			p->setPersistence(planet.pers);
			p->setBaseResolution(planet.res * spinFactor);
			p->setLacunarity(planet.lac);
		}
		else if (control == THR_CTRL)
			scene->getPlanetById(planet.id)->setThreshold(planet.thresh);
		else if (control == MODE_CTRL)
			scene->getPlanetById(planet.id)->setNoiseMode(planet.mode);
		else if (control == LOD_CTRL)
			scene->getPlanetById(planet.id)->setLODFactor(planet.lod);
	}

public:
	static void initUI(Scene* s, int mainWindow);

	static void refresh();

	static void setStats(float fps, float frTime);

	static void setPatchesNr(int patchesN);

	static void setTest(float alt, float dist);

	static void hide();
};
