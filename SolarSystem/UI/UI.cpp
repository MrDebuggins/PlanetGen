#include "UI.h"


Scene* UI::scene;
GLUI* UI::glui;
bool UI::hidden = false;

GLUI_Rollout* UI::planetCtrl;
GLUI_Rollout* UI::noiseCtrl;
GLUI_Rollout* UI::layersCtrl;
GLUI_Listbox* UI::planets;

GLUI_Spinner* UI::planetRadius;
GLUI_Spinner* UI::planetPosX;
GLUI_Spinner* UI::planetPosY;
GLUI_Spinner* UI::planetPosZ;
GLUI_Spinner* UI::lodfactor;

GLUI_Spinner* UI::layersNr;
GLUI_Spinner* UI::baseAmpl;
GLUI_Spinner* UI::baseRes;
GLUI_Spinner* UI::persistence;
GLUI_Spinner* UI::lacunarity;


GLUI_Spinner* UI::threshold;
GLUI_Listbox* UI::modes;

GLUI_StaticText* UI::fpsCounter;
GLUI_StaticText* UI::frameTime;
GLUI_StaticText* UI::patches;
GLUI_StaticText* UI::dist;
GLUI_StaticText* UI::alt;

//GLUI_Spinner* UI::factor;

int UI::wireframe;
int UI::spinFactor = 1;
PlanetData UI::planet;


void UI::initUI(Scene* s, int mainWindow)
{
	glui = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_RIGHT);
	glui->set_main_gfx_window(mainWindow);
	spinFactor = 1000;

	hidden = false;
	scene = s;

	planetCtrl = new GLUI_Rollout(glui, "Planet", true);

	// planets listbox
	planets = new GLUI_Listbox(planetCtrl, "Planet:", &planet.id, -1, planetSelectCB);
	std::vector<std::string> names = scene->getPlanetNames();
	for(int i = 0; i < names.size(); ++i)
	{
		planets->add_item(i, names[i].c_str());
	}

	// planet radius
	planetRadius = new GLUI_Spinner(planetCtrl, "Radius:", &planet.radius, RADIUS_CTRL, planetFloats);
	planetRadius->set_speed(1);
	//planetRadius->set_float_limits(1, 30000);
	planetRadius->set_float_val(scene->getPlanetById(0)->getRadius() / spinFactor);

	// planet position
	planetPosX = new GLUI_Spinner(planetCtrl, "x:", &planet.posX, POS_CTRL, planetFloats);
	planetPosX->set_speed(spinFactor);
	planetPosX->set_float_val(scene->getPlanetById(0)->getPosition().x / spinFactor);

	planetPosY = new GLUI_Spinner(planetCtrl, "y:", &planet.posY, POS_CTRL, planetFloats);
	planetPosY->set_speed(spinFactor);
	planetPosY->set_float_val(scene->getPlanetById(0)->getPosition().y / spinFactor);

	planetPosZ = new GLUI_Spinner(planetCtrl, "z:", &planet.posZ, POS_CTRL, planetFloats);
	planetPosZ->set_speed(spinFactor);
	planetPosZ->set_float_val(scene->getPlanetById(0)->getPosition().z / spinFactor);

	lodfactor = new GLUI_Spinner(planetCtrl, "LOD factor:", &planet.lod, LOD_CTRL, planetFloats);
	lodfactor->set_speed(10);
	lodfactor->set_float_limits(2, 16);
	lodfactor->set_float_val(scene->getPlanetById(0)->getLODFactor());

	// noise layers
	noiseCtrl = new GLUI_Rollout(glui, "Noise:", false);

	modes = new GLUI_Listbox(noiseCtrl, "Mode:", &planet.mode, MODE_CTRL, planetFloats);
	modes->add_item(0, "Default");
	modes->add_item(1, "Ridged");
	modes->add_item(2, "Sine");
	modes->add_item(3, "Offset");
	modes->add_item(4, "Offset + Ridged");
	modes->add_item(5, "Sine + Ridged");

	threshold = new GLUI_Spinner(noiseCtrl, "Threshold:", &planet.thresh, THR_CTRL, planetFloats);
	threshold->set_float_val(scene->getPlanetById(0)->getThreshold());
	threshold->set_speed(5);
	threshold->set_float_limits(0, 1);

	layersNr = new GLUI_Spinner(noiseCtrl, "Nr:", &planet.layers, LAYER_CTRL, planetFloats);
	layersNr->set_speed(1);
	layersNr->set_int_limits(1, 10);
	layersNr->set_int_val(scene->getPlanetById(0)->getNoiseLayers());

	// amplitude
	baseAmpl = new GLUI_Spinner(noiseCtrl, "Ampl:", &planet.ampl, LAYER_CTRL, planetFloats);
	baseAmpl->set_speed(1);
	//baseAmpl->set_float_limits(0, 1000);
	baseAmpl->set_float_val(scene->getPlanetById(0)->getBaseAmplitude() / spinFactor);
	persistence = new GLUI_Spinner(noiseCtrl, "Pers:", &planet.pers, LAYER_CTRL, planetFloats);
	persistence->set_speed(1);
	//persistence->set_float_limits(1, 1000);
	persistence->set_float_val(scene->getPlanetById(0)->getPersistence());

	// resolution
	baseRes = new GLUI_Spinner(noiseCtrl, "Res:", &planet.res, LAYER_CTRL, planetFloats);
	baseRes->set_speed(1);
	//baseRes->set_float_limits(0, 1000);
	baseRes->set_float_val(scene->getPlanetById(0)->getBaseResolution() / spinFactor);
	lacunarity = new GLUI_Spinner(noiseCtrl, "Lac:", &planet.lac, LAYER_CTRL, planetFloats);
	lacunarity->set_speed(1);
	//lacunarity->set_float_limits(1, 1000);
	lacunarity->set_float_val(scene->getPlanetById(0)->getLacunarity());


	new GLUI_Checkbox(glui, "Wireframe", &wireframe, 1, wireframeCB);
	fpsCounter = new GLUI_StaticText(glui, "FPS");
	frameTime = new GLUI_StaticText(glui, "FrameTime");
	patches = new GLUI_StaticText(glui, "PathcesGPU");
	dist = new GLUI_StaticText(glui, "dist");
	alt = new GLUI_StaticText(glui, "alt");

	//factor = new GLUI_Spinner(glui, "Spinners factor", &spinFactor, 2, wireframeCB);
	//factor->set_speed(10);
	//factor->set_float_limits(1, 1000);
}

void UI::refresh()
{
	glui->refresh();
}

void UI::setStats(float fps, float frTime)
{
	std::string str = "FPS:";
	str += std::to_string(fps).substr(0, 4);
	fpsCounter->set_text(str.c_str());

	str = "Frame time: ";
	str += std::to_string(frTime) + "s";
	frameTime->set_text(str.c_str());
}

void UI::setPatchesNr(int patchesN)
{
	std::string str = "GPU patches: ";
	str += std::to_string(patchesN);
	patches->set_text(str.c_str());
}


void UI::setTest(float a, float d)
{
	std::string str = "Altitude: ";
	str += std::to_string(a);
	alt->set_text(str.c_str());

	str = "Distance";
	str += std::to_string(d);
	dist->set_text(str.c_str());
}

void UI::hide()
{
	if (hidden)
		glui->show();
	else
		glui->hide();

	hidden = !hidden;
}
