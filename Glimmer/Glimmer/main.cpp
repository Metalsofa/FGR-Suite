/* Main source file for Glimmer */
#include <algorithm>
#include "freeglut32.h"
#include "resource.h"
//The location on the computer where this session takes place (is a folder, not a file)
#include <string>
std::string sessionFilePath;

//Custom header includes
//#include "crypt.h"
#include "fgrutils.h"
#include "customgl.h"
#include "editor.h"

//STL/etc. includes
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <vector>


typedef std::list<editor> tabContainerType;
//Some global variables
//Every editor is in this vector
tabContainerType tabs;
//The current tab is pointed to by this
tabContainerType::iterator currentTab;

//Exits the program
void closeProgram();

//Returns true if and only if there are no unsaved changes in any tab
bool sessionSaved() {
	for (tabContainerType::iterator itr = tabs.begin(); itr != tabs.end(); ++itr) {
		if (itr -> unsavedChanges) {
			return false;
		}
	}
	return true;
}

//Closes the tab at the iterator (doesn't check for saving!) and sets the tab iterator to the tab after it
void closeTab(tabContainerType::iterator& which) {
	which = tabs.erase(which);
	if (!tabs.size()) {
		closeProgram();
	}
}
#include "console.h"
#include "controls.h"


//#include <windows.h>
#include <GL/glut.h>

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
	//Draw the little console window
	cli::draw();

	glColor3f(1.0f, 1.0f, 1.0f);
	setViewport(superWindowPane(), false);
	outlineViewport(viewport(1, 1, superWindowPane().right(), superWindowPane().top()));

	//This is the function that refreshes the canvas and implements everything we've 'drawn'
	glutSwapBuffers();
}

//Initialize the editor using the command line arguments
void initTabs(int argc, char** argv) {
	//If this program was opened without a target file,
	if (argc == 1) {
		//Tabular setup
		tabs.push_back(editor(eGlyph));
		currentTab = tabs.begin();
		tabs.back().blankFile = true;
	}
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

	//Create the Window
	glutCreateWindow("GlimmerTitle");
	//Maximize the window
	glut32::maximizeWindow("GlimmerTitle");
	//Set the window icon (windows only)
	glut32::setWindowIcon("GlimmerTitle", argv[0], IDI_ICON1);
	//Change the window name to something better
	currentTab->updateWindowName();
	
	//Temporary debugging-esque stuff
	currentTab->glyphArt->mode = fgr::glLineStrip;

	std::vector<fgr::point> testVec;

	////Some settings
	//glutIgnoreKeyRepeat(1);
	////glutSetCursor(GLUT_CURSOR_NONE); //Hide the cursor

	//// Display callbacks
	glutDisplayFunc(renderScene); //Callback for when we refresh
	//glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize); //Callback for when window is resized

	//// Control callbacks
	glutKeyboardFunc(processNormalKeys); //Callback pressing a "normal" key
	////glutSpecialFunc(ProcessSpecialKeys); //Callback for a "special" key
	glutKeyboardUpFunc(releaseNormalKeys); //Callback for releasing "normal" keys
	////glutSpecialUpFunc(ReleaseSpecialKeys); //Callback for releasing special keys
	glutMouseFunc(MouseClick); //Callback for mouse clicks
	glutMotionFunc(ActiveMouseMove); //Callback for mouse movement with button down
	glutPassiveMotionFunc(PassiveMouseMove); //Callback for mouse movement with no button down

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