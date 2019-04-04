/* This header file defines some opengl rendering shortcuts */
#pragma once

#ifndef __customgl_h__
#define __customgl_h__

#include "fgrcolor.h"

//Global variables to help keep track of window size
GLsizei windowHeight;
GLsizei windowWidth;


//Clears the screen
void ClearScreen() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }


/////////////////////WARNING: MOSTLY BORING STUFF AFTER THIS LINE/////////////////////////

//Contains all gl-code; there should be no need to have any outside of this function
void renderScene(void);



//Callback for when the window changes size
void changeSize(int width, int height) {
	windowHeight = height;
	windowWidth = width;
	//To avoid divide by zero:
	if (height == 0)
		height = 1;
	float ratio = 1.0f * width / height;
	//Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	//Reset Matrix
	glLoadIdentity();
	//Multiply the current matrix by one that makes the view orthographic
	glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
	//Set viewport to be the entire window
	//glViewport(0, 0, width, height);
	//Get back to the modelview
	glMatrixMode(GL_MODELVIEW);
}

#endif
