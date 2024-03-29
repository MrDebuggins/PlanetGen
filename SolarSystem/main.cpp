﻿#include <iostream>

#include "Scene.h"
#include "Planet/Planet.h"

#include <GL/freeglut_ext.h>


// to implicitly run the app on high performance graphics card
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

Scene scene = Scene();
std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();


static void processKeyboardPress(const unsigned char key, const int x, const int y)
{
	scene.processKeyboard(key, x, y, true);
	glutPostRedisplay();
}

static void processKeyboardRelease(const unsigned char key, const int x, const int y)
{
	scene.processKeyboard(key, x, y, false);
	glutPostRedisplay();
}

static void processMouseMove(const int x, const int y)
{
	scene.processMouseMovement(x, y);
	glutPostRedisplay();
}

static void display()
{
	scene.draw();
	glutPostRedisplay();
}

static void update()
{
	scene.update();
}

int main(int argc, char** argv)
{
	// init parameters
	glutInit(&argc, argv);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(700, 400);
	glutCreateWindow("PlanetGen");
	
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(Renderer::reshape);
	glutIdleFunc(update);
	glutKeyboardFunc(processKeyboardPress);
	glutKeyboardUpFunc(processKeyboardRelease);
	glutPassiveMotionFunc(processMouseMove);

	scene.initRenderer();
	Planet test = Planet(6371000.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	//Planet test2 = Planet(1737400.0f, glm::vec3(360000000.0f, 0.0f, 0.0f));
	RandableObj test1 = RandableObj();
	scene.addPlanet(&test);
	//scene.addPlanet(&test2);
	scene.addObject(&test1);

	// start application
	glutFullScreen();
	glutMainLoop();
	
	glutExit();
	return 0;
}
