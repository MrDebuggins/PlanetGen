﻿#include <iostream>

#include "Planet/Planet.h"
#include <GL/freeglut_ext.h>

#include "UI/UI.h"


// to implicitly run the app on high performance graphics card
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int mainWindow, controlsWindow;
auto lastTime = std::chrono::high_resolution_clock::now();

Scene scene = Scene();


static void processKeyboardPress(const unsigned char key, const int x, const int y)
{
	scene.processKeyboard(key, x, y, true);
}

static void processKeyboardRelease(const unsigned char key, const int x, const int y)
{
	scene.processKeyboard(key, x, y, false);
}

static void processMouseMove(const int x, const int y)
{
	scene.processMouseMovement(x, y);
}

static void reshape(const int w, const int h)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);
	Renderer::reshape(tw, th);
	glutPostRedisplay();
}

static void display()
{
	scene.draw();
	UI::refresh();
}

static void update()
{
	if (glutGetWindow() != mainWindow)
		glutSetWindow(mainWindow);
	scene.update();
	glutPostRedisplay();

	float frameTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - lastTime).count() / 1000000.0;
	UI::setStats(1 / frameTime, frameTime, scene.getPatchesToBeSentToGPU());
	lastTime = std::chrono::high_resolution_clock::now();
}

int main(int argc, char** argv)
{
	// init parameters
	glutInit(&argc, argv);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(700, 400);
	mainWindow = glutCreateWindow("PlanetGen");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// register freeglut callbacks
	glutDisplayFunc(display);
	glutKeyboardUpFunc(processKeyboardRelease);
	glutPassiveMotionFunc(processMouseMove);

	// register glui callbacks
	GLUI_Master.set_glutReshapeFunc(Renderer::reshape);
	GLUI_Master.set_glutKeyboardFunc(processKeyboardPress);
	GLUI_Master.set_glutIdleFunc(update);

	// add objects to scene
	scene.initRenderer();
	Planet test = Planet("Earth", 6371000.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	//Planet test = Planet("Earth", 2.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	scene.addPlanet(&test);
	RandableObj lmao;
	scene.addObject(&lmao);

	// init glui
	UI::initUI(&scene, mainWindow);

	// start application
	glutFullScreen();
	glutMainLoop();
	
	glutExit();
	return 0;
}
