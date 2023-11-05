#include "Scene.h"
#include "Surface/Planet.h"


extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}



Scene scene = Scene();
static void processKeyboard(const unsigned char key, const int x, const int y)
{
	scene.processKeyboard(key, x, y);
}

static void processMouseMove(const int x, const int y)
{
	scene.processMouseMovement(x, y);
}

static void display()
{
	scene.draw();
}

static void reshape(const int w, const int h)
{
	scene.reshape(w, h);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(700, 400);
	glutCreateWindow("PlanetGen");	
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	scene.initRenderer();
	Planet* test = new Planet(6371000.0f, glm::vec3(0, 0, 0));
	scene.addObject(test);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(processKeyboard);
	glutPassiveMotionFunc(processMouseMove);
	glutFullScreen();
	glutMainLoop();

	glutExit();
	return 0;
}
