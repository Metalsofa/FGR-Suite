/* Main source file for Glimmer */

//Custom header includes
#include "geometry.h"
#include "crypt.h"
#include "gl/glut.h"
#include "fgrtoolkit.h"
#include "customgl.h"
#include "editor.h"

//STL/etc. includes
#include <Windows.h>
#include <cmath>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>


typedef std::list<editor> tabContainerType;
//Some global variables
//The location on the computer where this session takes place (is a folder, not a file)
std::string sessionFilePath;
//Every editor is in this vector
tabContainerType tabs;
//The current tab is pointed to by this
tabContainerType::iterator currentTab;

#include "controls.h"


/*
 * GL02Primitive.cpp: Vertex, Primitive and Color
 * Draw Simple 2D colored Shapes: quad, triangle and polygon.
 */
#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h

 /* Initialize OpenGL Graphics */
void initGL() {
	// Set "clearing" or background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
}

//Contains all gl-code; there should be no need to have any outside of this function
void renderScene(void) {
	//Screen-cleanup
	// Clear Color and Depth Buffers
	ClearScreen();
	// Reset transformations
	glLoadIdentity();

	///////////////////////// DRAWING INSTRUCTIONS ////////////////////////////

	//Draw the current editor
	drawEditor(*currentTab);

	setViewport(superWindowPane(), false);
	outlineViewport(viewport(1,1,superWindowPane().right(), superWindowPane().top()));

	//This is the function that refreshes the canvas and implements everything we've 'drawn'
	glutSwapBuffers();
}

//Initialize the editor using the command line arguments
void initTabs(int argc, char** argv) {
	//Tabular setup
	tabs.push_back(editor(eAnimation));
	currentTab = tabs.begin();
}

//main function; exists to set up a few things and then enter the glut-main-loop
int main(int argc, char** argv) {
	//Command line args:
	initTabs(argc, argv);

	//Initialize GLUT
	glutInit(&argc, argv);

	////Choose some settings for our Window
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(900, 500);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	////Create the Window
	glutCreateWindow("Glimmer");

	////Some settings
	//glutIgnoreKeyRepeat(1);
	////glutSetCursor(GLUT_CURSOR_NONE); //Hide the cursor

	//// Display callbacks
	glutDisplayFunc(renderScene); //Callback for when we refresh
	glutReshapeFunc(changeSize); //Callback for when window is resized

	//// Control callbacks
	glutKeyboardFunc(processNormalKeys); //Callback pressing a "normal" key
	////glutSpecialFunc(ProcessSpecialKeys); //Callback for a "special" key
	glutKeyboardUpFunc(releaseNormalKeys); //Callback for releasing "normal" keys
	////glutSpecialUpFunc(ReleaseSpecialKeys); //Callback for releasing special keys
	////glutMouseFunc(MouseClick); //Callback for mouse clicks
	////glutMotionFunc(PassiveMouseMove); //Callback for mouse movement with button down
	////glutPassiveMotionFunc(PassiveMouseMove); //Callback for mouse movement with no button down

	//initGL();
	//
	////enter GLUT event processing cycle
	//glutMainLoop();
	
	//glutInit(&argc, argv);          // Initialize GLUT
	//glutCreateWindow("Vertex, Primitive & Color");  // Create window with the given title
	//glutInitWindowSize(320, 320);   // Set the window's initial width & height
	//glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	//glutDisplayFunc(display);       // Register callback handler for window re-paint event
	initGL();                       // Our own OpenGL initialization
	glutMainLoop();                 // Enter the event-processing loop


	return 0;
}
