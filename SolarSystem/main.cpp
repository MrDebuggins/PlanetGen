#include <iostream>

#include "Planet/Planet.h"
#include <GL/freeglut_ext.h>

#include "Perlin.h"
#include "TestObj.h"
#include "UI/UI.h"


// to implicitly run the app on high performance graphics card
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int mainWindow, controlsWindow;
auto lastTime = std::chrono::high_resolution_clock::now();
float timer1 = 0;
float frameTime = 0;

Scene scene = Scene();


static void processKeyboardPress(const unsigned char key, const int x, const int y)
{
	if (key == 'h')
		UI::hide();

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

static void processMouseWheel(int btn, int direction, int x, int y)
{
	scene.processMouseWheel(direction);
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

	frameTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - lastTime).count() / 1000000.0;
	lastTime = std::chrono::high_resolution_clock::now();

	scene.update(frameTime);

	UI::setPatchesNr(scene.getPatchesToBeSentToGPU());
	timer1 += frameTime;
	if(timer1 > 0.5)
	{
		UI::setStats(1 / frameTime, frameTime);
		timer1 = 0;
	}

	// test code _________________________________________
	float d = glm::length(coef_M * scene.camera.position_M + scene.camera.position_m);
	UI::setTest(scene.testFunc(), d);
	// test code _________________________________________

	glutPostRedisplay();
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
	glutMouseWheelFunc(processMouseWheel);

	// register glui callbacks
	GLUI_Master.set_glutReshapeFunc(Renderer::reshape);
	GLUI_Master.set_glutKeyboardFunc(processKeyboardPress);
	GLUI_Master.set_glutIdleFunc(update);

	// add objects to scene
	scene.initRenderer();
	Planet test = Planet("Earth", 6371000.0f, glm::vec3(0.0f, 0.0f, 0.0f), &scene.camera);
	scene.addPlanet(&test);
	//RandableObj* lmao = new TestObj();
	//scene.addObject(lmao);

	// init glui
	UI::initUI(&scene, mainWindow);

	// start application
	glutFullScreen();
	glutMainLoop();
	
	glutExit();
	return 0;
}
