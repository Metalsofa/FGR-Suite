//This header file defines the manner in which the user uses the keyboard and mouse to manipulate the program
#pragma once

#ifndef __controls_h__
#define __controls_h__

//Global control variables
//Note that these are not booleans; if evaluated as such, *down* evaluates to false.
	//(Now declared in main document)

//Where the mouse was when we last checked, in the order x, y
int mouseMemory [2];

//GLUT event handler for regular key-presses
void processNormalKeys(unsigned char key, int x, int y) {
	if (cli::listening) {
		// <CR>
		if (key == 13) {
			cli::gulp();
			cli::listening = false;
		}
		// <backspace>
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
		return;
	}
	if (key == ':') {
		cli::listening = true;
		cli::showLastMessage = false;
		cli::input.push_back(':');
	}
	if (key == '0')
		cli::digest("fit");
	renderScene();
	return;
}

//GLUT event handler for regular key-releases
void releaseNormalKeys(unsigned char key, int x, int y) {

}

//This function is called every time a left-click occurs. By this time, the state will be updated.
void leftClick(int x, int y) {
	//Switch for if this is click-press or click-release
	switch (mouseStates[GLUT_LEFT_BUTTON]) {
	case GLUT_DOWN:
		//Behaviour depends on what pane has been clicked
		switch (currentTab->reigonID(x, y)) {
		case rCentral:
			//Action depends on tool
			switch (currentTab->currentTool) {
				case tAppend:
				currentTab->pushBackPoint(x, y);
				renderScene();
				return;
			}
		case rAnimationFrames:
			cli::send_message("Animation Frames");
			return;
		case rCommandLine:
			cli::send_message("Command Line");
			return;
		case rFileTree:
			cli::send_message("File Tree");
			return;
		case rGlyphGLMode:
			cli::send_message("Glyph GL Mode");
			return;
		case rLayers:
			cli::send_message("Layers");
			return;
		case rShapeProperties:
			cli::send_message("Shape Properties");
			return;
		case rShapes:
			cli::send_message("Shapes");
			return;
		case rTabHeader:
			cli::send_message("Tab Header");
			return;
		case rTools:
			cli::send_message("Tools");
			return;
		case rInconclusive:
			cli::send_message("Inconclusive");
			break;
		default:
			cli::send_message("DEFAULT");
			break;
		}
	case GLUT_UP:

		break;
	}
}

//This function is called every time a middle-click occurs. By this time, the state will be updated.
void middleClick(int x, int y) {
	//Behaviour depends on what pane the motion is in
	switch (currentTab->reigonID(x, y)) {
	case rCentral:
		//Action depends on tool
		switch (currentTab->currentTool) {
			case tAppend:
			return;
		}
		return;
	case rAnimationFrames:
		return;
	case rCommandLine:
		return;
	case rFileTree:
		return;
	case rGlyphGLMode:
		return;
	case rLayers:
		return;
	case rShapeProperties:
		return;
	case rShapes:
		return;
	case rTabHeader:
		return;
	case rTools:
		return;
	case rInconclusive:
		break;
	default:
		break;
	}
}

//This function is called every time a right-click occurs. By this time, the state will be updated.
void rightClick(int x, int y) {
	//Behaviour depends on what pane is clicked inside of
	switch (currentTab->reigonID(x, y)) {
	case rCentral:
		//Action depends on tool
		switch (currentTab->currentTool) {
		case tAppend:
			if (currentTab->currentGlyph().size()) {
				currentTab->currentGlyph().back() = currentTab->mapPixel(x, y);
				renderScene();
			}
			break;
		}
		break;
	case rAnimationFrames:
		break;
	case rCommandLine:
		break;
	case rFileTree:
		break;
	case rGlyphGLMode:
		break;
	case rLayers:
		break;
	case rShapeProperties:
		break;
	case rShapes:
		break;
	case rTabHeader:
		break;
	case rTools:
		break;
	case rInconclusive:
		break;
	default:
		break;
	}
	return;
}

//GLUT event handler for a mouse click
void MouseClick(int button, int state, int x, int y) {
	//Toggling - this always happens
	if (button < 3)
		mouseStates[button] = state;
	//Other activated controls
	switch (button) {
	case GLUT_LEFT_BUTTON:
		leftClick(x, y);
		break;
	case GLUT_MIDDLE_BUTTON:
		middleClick(x, y);
		break;
	case GLUT_RIGHT_BUTTON:
		rightClick(x, y);
		break;
	case 3: //SCROLL UP
		if (state == GLUT_DOWN) {
			switch (currentTab->reigonID(x, y)) {
			case rCentral:
				currentTab->zoomIn();
				renderScene();
				break;
			}
		}
		break;
	case 4: //SCROLL DOWN
		if (state == GLUT_DOWN) {
			switch (currentTab->reigonID(x, y)) {
			case rCentral:
				currentTab->zoomOut();
				renderScene();
				break;
			}
		}
			break;
	}
	//Update mouse memory
	mouseMemory[0] = x; mouseMemory[1] = y;
	return;
}

//GLUT event handler for mouse motion with a mouse-button down
void ActiveMouseMove(int x, int y) {
	//Behaviour depends on what pane the motion is in
	switch (currentTab->reigonID(x, y)) {
	case rCentral:
		//If both not both mouse buttons are down, move the last point to the mouse
		if (!(mouseStates[GLUT_RIGHT_BUTTON] && mouseStates[GLUT_LEFT_BUTTON])) {
			//If there is a point at all,
			if (currentTab->currentGlyph().size()) {
				currentTab->currentGlyph().back() = currentTab->mapPixel(x, y);
				renderScene();
			}
		}
		//If the middle-button is down,
		if (!mouseStates[GLUT_MIDDLE_BUTTON]) {
			//Pan an amount equal to the mouse motion
			currentTab->pan -= (currentTab->mapPixel(x, y) - currentTab->mapPixel(mouseMemory[0], mouseMemory[1])) * currentTab->zoom;
			renderScene();
		}
		break;
	case rAnimationFrames:
		break;
	case rCommandLine:
		break;
	case rFileTree:
		break;
	case rGlyphGLMode:
		break;
	case rLayers:
		break;
	case rShapeProperties:
		break;
	case rShapes:
		break;
	case rTabHeader:
		break;
	case rTools:
		break;
	case rInconclusive:
		break;
	default:
		break;
	}
	//Update mouse memory
	mouseMemory[0] = x; mouseMemory[1] = y;
	return;
}

//GLUT event handler for mouse motion with no mouse-button down
void PassiveMouseMove(int x, int y) {

	//Update mouse memory
	mouseMemory[0] = x; mouseMemory[1] = y;
	return;
}

#endif
