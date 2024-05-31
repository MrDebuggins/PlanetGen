#pragma once
#include "../Scene.h"
#include <GL/glui.h>


#define RADIUS_CTRL 1
#define POS_CTRL 2
#define LAYER_CTRL 3

struct PlanetData
{
	int id;
	float radius;
	float posX, posY, posZ;

	float ampl0, res0;
	float ampl1, res1;
	float ampl2, res2;
	float ampl3, res3;
	float ampl4, res4;
};

class UI
{
	static Scene* scene;
	static GLUI* glui;

	// controls
	static GLUI_Rollout *planetCtrl;
	static GLUI_Rollout *noiseCtrl;
	static GLUI_Listbox *planets;

	static GLUI_Spinner* planetRadius;
	static GLUI_Spinner* planetPosX;
	static GLUI_Spinner* planetPosY;
	static GLUI_Spinner* planetPosZ;

	static GLUI_Spinner* layer0Ampl;
	static GLUI_Spinner* layer1Ampl;
	static GLUI_Spinner* layer2Ampl;
	static GLUI_Spinner* layer3Ampl;
	static GLUI_Spinner* layer4Ampl;
	static GLUI_Spinner* layer0Res;
	static GLUI_Spinner* layer1Res;
	static GLUI_Spinner* layer2Res;
	static GLUI_Spinner* layer3Res;
	static GLUI_Spinner* layer4Res;

	static GLUI_StaticText* fpsCounter;
	static GLUI_StaticText* frameTime;
	static GLUI_StaticText* patches;

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
		//	layer0Ampl->set_speed(spinFactor);
		//	layer0Res->set_speed(spinFactor);
		//	layer1Ampl->set_speed(spinFactor);
		//	layer1Res->set_speed(spinFactor);
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

		planetPosX->set_float_val(scene->getPlanetById(planet.id)->getPosition().x);
		planetPosY->set_float_val(scene->getPlanetById(planet.id)->getPosition().y);
		planetPosZ->set_float_val(scene->getPlanetById(planet.id)->getPosition().z);

		layer0Ampl->set_float_val(scene->getPlanetById(planet.id)->getAmplitudes()[0]);
		layer1Ampl->set_float_val(scene->getPlanetById(planet.id)->getAmplitudes()[1]);
		layer2Ampl->set_float_val(scene->getPlanetById(planet.id)->getAmplitudes()[2]);
		layer3Ampl->set_float_val(scene->getPlanetById(planet.id)->getAmplitudes()[3]);
		layer4Ampl->set_float_val(scene->getPlanetById(planet.id)->getAmplitudes()[4]);
		layer0Res->set_float_val(scene->getPlanetById(planet.id)->getPeriods()[0]);
		layer1Res->set_float_val(scene->getPlanetById(planet.id)->getPeriods()[1]);
		layer2Res->set_float_val(scene->getPlanetById(planet.id)->getPeriods()[2]);
		layer3Res->set_float_val(scene->getPlanetById(planet.id)->getPeriods()[3]);
		layer4Res->set_float_val(scene->getPlanetById(planet.id)->getPeriods()[4]);
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
			float ampl[] = { planet.ampl0, planet.ampl1, planet.ampl2, planet.ampl3, planet.ampl4 };
			float res[] = { planet.res0, planet.res1, planet.res2, planet.res3, planet.res4 };
			for(int i = 0; i < 5; ++i)
			{
				ampl[i] *= spinFactor;
				res[i] *= spinFactor;
			}
			scene->getPlanetById(planet.id)->setAmplitudes(ampl);
			scene->getPlanetById(planet.id)->setPeriods(res);
		}
	}

public:
	static void initUI(Scene* s, int mainWindow);

	static void refresh();

	static void setStats(float fps, float frTime, int patchesN);
};
