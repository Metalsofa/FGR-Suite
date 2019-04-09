/* Main source file for Glimmer */
#include <algorithm>
#include "freeglut32.h"
#include "resource.h"
//The location on the computer where this session takes place (is a folder, not a file)
#include <string>
#include <fstream>

bool zen = false;

std::string sessionFilePath;

//Custom header includes
//#include "crypt.h"
#include "fgrutils.h"
GLint mouseStates[3] = { GLUT_UP, GLUT_UP, GLUT_UP };
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
	glLineWidth(1.0f);
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
		tabs.push_back(editor(eGraphic));
		currentTab = tabs.begin();
		tabs.back().blankFile = true;
		cli::send_message("Started session without arguments.");
		return;
	}
	//Otherwise, its reasonable to assume there's another argument
	tabs.push_back(editor(std::string(argv[1])));
	currentTab = tabs.begin();
	return;
}

//main function; exists to set up a few things and then enter the glut-main-loop
int main(int argc, char** argv) {
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

	//Command line args:
	initTabs(argc, argv);

	////Some settings
	//glutIgnoreKeyRepeat(1);
	////glutSetCursor(GLUT_CURSOR_NONE); //Hide the cursor

	//// Display callbacks
	glutDisplayFunc(renderScene); //Callback for when we refresh
	//glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize); //Callback for when window is resized

	//// Control callbacks
	glutKeyboardFunc(processNormalKeys); //Callback pressing a "normal" key
	glutSpecialFunc(ProcessSpecialKeys); //Callback for a "special" key
	glutSpecialUpFunc(ReleaseSpecialKeys); //Callback for releasing special keys
	glutKeyboardUpFunc(releaseNormalKeys); //Callback for releasing "normal" keys
	glutMouseFunc(MouseClick); //Callback for mouse clicks
	glutMotionFunc(ActiveMouseMove); //Callback for mouse movement with button down
	glutPassiveMotionFunc(PassiveMouseMove); //Callback for mouse movement with no button down

	//Attempt to load user prefrences
	std::ifstream prefstream(".glimrc");
	if (prefstream.is_open()) {
		cli::gulp(prefstream);
		prefstream.close();
	}

	initGL();                       // Our own OpenGL initialization
	glutMainLoop();                 // Enter the event-processing loop


	return 0;
}