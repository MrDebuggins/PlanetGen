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

GLUI_Spinner* UI::layer0Ampl;
GLUI_Spinner* UI::layer1Ampl;
GLUI_Spinner* UI::layer2Ampl;
GLUI_Spinner* UI::layer3Ampl;
GLUI_Spinner* UI::layer4Ampl;
GLUI_Spinner* UI::layer0Res;
GLUI_Spinner* UI::layer1Res;
GLUI_Spinner* UI::layer2Res;
GLUI_Spinner* UI::layer3Res;
GLUI_Spinner* UI::layer4Res;

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

	threshold = new GLUI_Spinner(noiseCtrl, "Threshold:", &planet.thresh, THR_CTRL, planetFloats);
	threshold->set_float_val(scene->getPlanetById(0)->getThreshold());
	threshold->set_speed(5);
	threshold->set_float_limits(0, 1);

	layersCtrl = new GLUI_Rollout(noiseCtrl, "Layers:", false);

	// layer 0
	layer0Ampl = new GLUI_Spinner(layersCtrl, "A0:", &planet.ampl0, LAYER_CTRL, planetFloats);
	layer0Ampl->set_speed(1);
	//layer0Ampl->set_float_limits(0, 1000);
	layer0Ampl->set_float_val(scene->getPlanetById(0)->getAmplitudes()[0] / spinFactor);
	layer0Res = new GLUI_Spinner(layersCtrl, "R0:", &planet.res0, LAYER_CTRL, planetFloats);
	layer0Res->set_speed(1);
	//layer0Res->set_float_limits(1, 1000);
	layer0Res->set_float_val(scene->getPlanetById(0)->getPeriods()[0] / spinFactor);

	// layer 1
	layer1Ampl = new GLUI_Spinner(layersCtrl, "A1:", &planet.ampl1, LAYER_CTRL, planetFloats);
	layer1Ampl->set_speed(1);
	//layer1Ampl->set_float_limits(0, 1000);
	layer1Ampl->set_float_val(scene->getPlanetById(0)->getAmplitudes()[1] / spinFactor);
	layer1Res = new GLUI_Spinner(layersCtrl, "R1:", &planet.res1, LAYER_CTRL, planetFloats);
	layer1Res->set_speed(1);
	//layer1Res->set_float_limits(1, 1000);
	layer1Res->set_float_val(scene->getPlanetById(0)->getPeriods()[1] / spinFactor);

	// layer 2
	layer2Ampl = new GLUI_Spinner(layersCtrl, "A2:", &planet.ampl2, LAYER_CTRL, planetFloats);
	layer2Ampl->set_speed(1);
	//layer2Ampl->set_float_limits(0, 1000);
	layer2Ampl->set_float_val(scene->getPlanetById(0)->getAmplitudes()[2] / spinFactor);
	layer2Res = new GLUI_Spinner(layersCtrl, "R2:", &planet.res2, LAYER_CTRL, planetFloats);
	layer2Res->set_speed(1);
	//layer2Res->set_float_limits(1, 1000);
	layer2Res->set_float_val(scene->getPlanetById(0)->getPeriods()[2] / spinFactor);

	// layer 3
	layer3Ampl = new GLUI_Spinner(layersCtrl, "A3:", &planet.ampl3, LAYER_CTRL, planetFloats);
	layer3Ampl->set_speed(1);
	//layer3Ampl->set_float_limits(0, 1000);
	layer3Ampl->set_float_val(scene->getPlanetById(0)->getAmplitudes()[3] / spinFactor);
	layer3Res = new GLUI_Spinner(layersCtrl, "R3:", &planet.res3, LAYER_CTRL, planetFloats);
	layer3Res->set_speed(1);
	//layer3Res->set_float_limits(1, 1000);
	layer3Res->set_float_val(scene->getPlanetById(0)->getPeriods()[3] / spinFactor);

	// layer 4
	layer4Ampl = new GLUI_Spinner(layersCtrl, "A4:", &planet.ampl4, LAYER_CTRL, planetFloats);
	layer4Ampl->set_speed(1);
	//layer4Ampl->set_float_limits(0, 1000);
	layer4Ampl->set_float_val(scene->getPlanetById(0)->getAmplitudes()[4] / spinFactor);
	layer4Res = new GLUI_Spinner(layersCtrl, "R4:", &planet.res4, LAYER_CTRL, planetFloats);
	layer4Res->set_speed(1);
	//layer4Res->set_float_limits(1, 1000);
	layer4Res->set_float_val(scene->getPlanetById(0)->getPeriods()[4] / spinFactor);


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
