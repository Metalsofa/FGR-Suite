/* Main source file for Glimmer */

//Custom header includes
#include "geometry.h"
#include "crypt.h"
#include "gl/glut.h"
#include "graphics.h"
#include "customgl.h"

//STL/etc. includes
#include <Windows.h>
#include <cmath>
#include <stdlib.h>
#include <string>
#include <map>
#include <time.h>
#include <vector>

using namespace std;

//Some global variables

//Contains all gl-code; there should be no need to have any outside of this function
void renderScene(void) {
	//Screen-cleanup
	// Clear Color and Depth Buffers
	ClearScreen();
	// Reset transformations
	glLoadIdentity();

	//This is the function that refreshes the canvas and implements everything we've 'drawn'
	glutSwapBuffers();
}

//main function; exists to set up a few things and then enter the glut-main-loop
int main(int argc, char** argv) {
	//Command line args:

	//Initialize GLUT
	glutInit(&argc, argv);

	//Choose some settings for our Window
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(900, 500);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	//Create the Window
	glutCreateWindow("Glimmer");
	glutFullScreen();

	//Some settings
	glutIgnoreKeyRepeat(1);
	glutSetCursor(GLUT_CURSOR_NONE); //Hide the cursor

	// Display callbacks
	glutDisplayFunc(renderScene); //Callback for when we refresh
	glutReshapeFunc(changeSize); //Callback for when window is resized

	// Control callbacks
	//glutKeyboardFunc(ProcessNormalKeys); //Callback pressing a "normal" key
	//glutSpecialFunc(ProcessSpecialKeys); //Callback for a "special" key
	//glutKeyboardUpFunc(ReleaseNormalKeys); //Callback for releasing "normal" keys
	//glutSpecialUpFunc(ReleaseSpecialKeys); //Callback for releasing special keys
	//glutMouseFunc(MouseClick); //Callback for mouse clicks
	//glutMotionFunc(PassiveMouseMove); //Callback for mouse movement with button down
	//glutPassiveMotionFunc(PassiveMouseMove); //Callback for mouse movement with no button down

	//enter GLUT event processing cycle
	glutMainLoop();

	return 0;
}
