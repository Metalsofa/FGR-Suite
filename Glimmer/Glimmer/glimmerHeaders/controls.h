#pragma once

#ifndef __controls_h__
#define __controls_h__

#include <map>

//The console is not tab-specific, but session-specific.
bool consoleMode = false;

void consoleGulp() {

}


//GLUT event handler for regular key-presses
void processNormalKeys(unsigned char key, int x, int y) {
	if (consoleMode) {
		if (key == 13)
			consoleGulp();

	}
	if (key == ':')
		consoleMode = true;
}

//GLUT event handler for regular key-releases
void releaseNormalKeys(unsigned char key, int x, int y) {

}


#endif
