#pragma once

#ifndef __console_h__
#define __console_h__

#include <vector>
#include <string>
#include <sstream>

//Urgency codes for system messages
enum uCode { uSuccess, uInvalid, uIncorrectUsage, uWarning, uError };

// Command-line interface namespace - session variables, not tab variables
namespace cli {
	//The console is not tab-specific, but session-specific.
	bool listening = false;
	bool showLastMessage = false;
	//The console history, including only user input (reverse order, of course)
	std::vector<std::string> history;
	//Message history in the console
	std::vector<std::string> messages;
	//The text currently in the console
	std::string input = std::string();
	//The prefrence set as to the backgorund color of the command line
	fgr::fcolor backColor(0.0f, 0.0f, 0.0f);
	//Interpret and execute the commands from the input
	uCode digest(const std::string& command);
	//Digest the CLI input
	void gulp() {
		history.push_back(input);
		input.erase(input.begin());
		digest(input);
		input.clear();
	}
	//Send a message to the console
	void send_message(const std::string& mess, uCode urgency = uSuccess) {
		if (urgency) {
			messages.push_back('<' + std::to_string(urgency) + '>' + mess);
		}
		else {
			messages.push_back(mess);
		}
		showLastMessage = true;
	}
	//The string that should show up on the command-line bar presently
	std::string getfield() {
		if (listening) {
			return input;
		}
		else {
			if (messages.size() > 0 && showLastMessage)
				return messages.back();
		}
		return std::string();
	}
	//Draw the console
	void draw();
	//Send a warning message that there are unsaved changes to the current tab
	void warnUnsaved();
}

//Exit the program
void closeProgram() {
	cli::send_message("Exiting...");
	exit(0);
}

//Send a warning message that there are unsaved changes to the current tab
void cli::warnUnsaved() {
	send_message("No write since last change (use ! to force)", uWarning);
}

//Interpret and execute the commands from the input
uCode cli::digest(const std::string& token) {
	if (!token.size())
		return uSuccess;
	std::stringstream input(token);
	std::string command;
	input >> command;
	//Force quit
	if (command == "q!") {
		std::string filename(splitPath(currentTab->filepath).second);
		closeTab(currentTab);
		send_message("Closed tab '" + filename + '\'');
		return uSuccess;
	}
	//Quit
	if (command == "q") {
		if (currentTab->unsavedChanges && !currentTab->blankFile) {
			warnUnsaved();
			return uWarning;
		}
		else {
			std::string filename(splitPath(currentTab->filepath).second);
			closeTab(currentTab);
			send_message("Closed tab '" + filename + '\'');
			return uSuccess;
		}
	}
	//Write
	if (command == "w") {
		//If the user provides a filename,
		if (input >> command) {
			if (!currentTab->unsavedChanges && command == currentTab->getFileName()) {
				send_message("No changes since last write");
				return uSuccess;
			}
			if (currentTab->save(command)) {
				currentTab->unsavedChanges = false;
				currentTab->updateWindowName();
				send_message("'" + command + "' written successfully");
				return uSuccess;
			}
			else {
				send_message("Error writing to '" + command + '\'', uError);
				return uError;
			}
		}
		if (!currentTab->unsavedChanges) {
			send_message("No changes since last write");
			return uSuccess;
		}
		if (currentTab->save()) {
			currentTab->unsavedChanges = false;
			currentTab->updateWindowName();
			send_message("'" + currentTab->getFileName() + "' written successfully");
			return uSuccess;
		}
		else {
			send_message("Error writing to '" + currentTab->getFileName() + '\'', uError);
			return uError;
		}
	}
	//Edit (open)
	if (command == "e") {
		//Ensure a filename was specified
		if (!(input >> command)) {
			send_message("Usage is :edit <filename>", uIncorrectUsage);
			return uIncorrectUsage;
		}
		//Ensure changes were saved
		if (!currentTab->unsavedChanges || currentTab->blankFile) {
			//Try to load an existing file with the given name
			if (currentTab->loadFile(command)) {
				send_message("Editing file '" + command + '\'', uSuccess);
				currentTab->updateWindowName();
				return uSuccess;
			}
			//Otherwise make a new file
			else {
				//Ensure the provided file extention is valid
				if (interpretExtention(getExtention(command))) {
					//Make a new file of the requested type
					send_message("Editing new file '" + command + '\'', uSuccess);
					currentTab->newFile(interpretExtention(getExtention(command)));
					currentTab->filepath = command;
					currentTab->updateWindowName();
					return uSuccess;
				}
				else {
					send_message("Unrecognized file extention - '."
						+ getExtention(command) + '\'', uError);
					return uError;
				}
			}
		}
		else {
			warnUnsaved();
			return uWarning;
		}
	}
	//Reset all transformations
	if (command == "home" || command == "h") {
		currentTab->zoom = 0.1f;
		currentTab->pan = fgr::point();
		currentTab->rotation = 0.0f;
		return uSuccess;
	}
	//Transform view to fit full graphic
	if (command == "fit") {
		if (currentTab->currentGlyph().size()) {
			fgr::segment fitSeg = currentTab->currentGlyph().bounds();
			currentTab->zoom = 1.0f / (fmaxf(fitSeg.width(), fitSeg.height() / currentTab->aspectRatio()));
			currentTab->pan = currentTab->currentGlyph().bounds().midpoint() * currentTab->zoom;
			return uSuccess;
		}
		return uSuccess;
	}
	//Change current glyph GL Mode
	if (command == "mode") {
		//Ensure another mode was specified
		if (input >> command) {
			int interpretation = (std::stoi(command) % 10);
			//For now, interpret as plan integer
			currentTab->currentGlyph().mode = static_cast<fgr::GLmode>(interpretation);

			send_message("Glyph GL Mode set to " + std::string(currentTab->currentGlyph().glModeString()), uSuccess);
			return uSuccess;
		}
		else {
			send_message("Usage is :mode <GLModename/GLModeNum>", uIncorrectUsage);
			return uIncorrectUsage;
		}
	}
	if (command == "color") {
		if (currentTab->format != eGlyph) {
			//If more arguments are supplied,
			float newR, newG, newB;
			if (input >> newR && input >> newG && input >> newB) {
				float newA;
				if (input >> newA) {
					currentTab->currentShape().color = fgr::fcolor(newR, newG, newB, newA);
					return uSuccess;
				}
				else {
					currentTab->currentShape().color = fgr::fcolor(newR, newG, newB);
					return uSuccess;
				}
			}
			//Otherwise, tell them what's what
			else {
				send_message("{ " +
					std::to_string(currentTab->currentShape().color.getLevel('r')) + ", " +
					std::to_string(currentTab->currentShape().color.getLevel('g')) + ", " +
					std::to_string(currentTab->currentShape().color.getLevel('b')) +
					" }", uSuccess);
				return uSuccess;
			}
		}
		else {
			send_message("Current art has no member 'color'", uError);
			return uError;
		}
	}
	//Err - invalid command
	send_message("Invalid command - " + command, uInvalid);
	return uInvalid;
}


//Rendering instructions for the console
void cli::draw() {
	setcolor(currentTab -> commandLineColor);
	setViewport(currentTab -> commandLinePane());
	std::string field = cli::getfield();
	for (int i = 0; i < field.size(); ++i)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, field[i]);
}

#endif