//This header file defines the manner in which the user uses the keyboard and mouse to manipulate the program
#pragma once

#ifndef __controls_h__
#define __controls_h__

//GLUT event handler for regular key-presses
void processNormalKeys(unsigned char key, int x, int y) {
	if (cli::listening) {
		if (key == 13) {
			cli::gulp();
			cli::listening = false;
		}
		else if (key == 8) {
			if (cli::input.size()) {
				cli::input.pop_back();
				if (!cli::input.size())
					cli::listening = false;
			}
		}
		else {
			cli::input.push_back(key);
		}

	}
	if (key == ':') {
		cli::listening = true;
		cli::input.push_back(':');
	}
	renderScene();
}

//GLUT event handler for regular key-releases
void releaseNormalKeys(unsigned char key, int x, int y) {

}


#endif
