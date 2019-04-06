//This header file defines the manner in which the user uses the keyboard and mouse to manipulate the program
#pragma once

#ifndef __controls_h__
#define __controls_h__

//Global control variables
//Note that these are not booleans; if evaluated as such, *down* evaluates to false.
GLint mouseStates[3] = { GLUT_UP, GLUT_UP, GLUT_UP };

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
		cli::showLastMessage = false;
		cli::input.push_back(':');
	}
	renderScene();
}

//GLUT event handler for regular key-releases
void releaseNormalKeys(unsigned char key, int x, int y) {

}

//This function is called every time a left-click occurs. By this time, the state will be updated.
void leftClick(int x, int y) {
	//Behaviour depends on what pane has been clicked
	switch (currentTab->reigonID(x, y)) {
	case rCentral:
		currentTab->pushBackPoint(x, y);
		return;
	case rInconclusive:
		break;
	default:
		break;
	}
}

//This function is called every time a middle-click occurs. By this time, the state will be updated.
void middleClick(int x, int y) {
	
}

//This function is called every time a right-click occurs. By this time, the state will be updated.
void rightClick(int x, int y) {
	
}

//GLUT event handler for a mouse click
void MouseClick(int button, int state, int x, int y) {
	//Toggling - this always happens
	mouseStates[button] = state;
	//Other activated controls
	switch (button) {
	case GLUT_LEFT_BUTTON:
		leftClick(x, y);
	case GLUT_MIDDLE_BUTTON:
		middleClick(x, y);
	case GLUT_RIGHT_BUTTON:
		rightClick(x, y);
	}
}

//GLUT event handler for mouse motion with a mouse-button down
void ActiveMouseMove(int x, int y) {

}

//GLUT event handler for mouse motion with no mouse-button down
void PassiveMouseMove(int x, int y) {

}

#endif
