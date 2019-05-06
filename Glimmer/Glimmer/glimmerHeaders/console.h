#pragma once

#ifndef __console_h__
#define __console_h__

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>

//All keymappings
std::map<std::string, std::string> bindings;
//User variables (bash-style; they only hold text)
std::unordered_map<std::string, std::string> user_variables;


//Urgency codes for system messages
enum uCode { uSuccess, uInvalid, uIncorrectUsage, uWarning, uError };

//Convert a string into an editortype
editortype stringToType(const std::string& text) {
	if (text == "glyph" || text == "fgl" || text == ".fgl")
		return eGlyph;
	if (text == "shape" || text == "fsh" || text == ".fsh")
		return eShape;
	if (text == "graphic" || text == "fgr" || text == ".fgr")
		return eGraphic;
	if (text == "animation" || text == "fan" || text == ".fan")
		return eAnimation;
	if (text == "spritesheet" || text == "fss" || text == ".fss")
		return eSpritesheet;
	return eNULL;
}

// Command-line interface namespace - session variables, not tab variables
namespace cli {
	typedef std::list<char> manipstring;
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
	//Parse a string and replace variables with their contents
	std::string var_expand(const std::string& words) {
		manipstring medium;
		for (unsigned int i = 0; i < words.size(); ++i)
			medium.push_back(words[i]);
		for (std::list<char>::iterator travis = medium.begin(); 
			travis != medium.end(); 
			++travis) {
			if (*travis == '$') {
				travis = medium.erase(travis);
				std::string keyword;
				while (travis != medium.end() && *travis != ' ') {
					keyword.push_back(*travis);
					travis = medium.erase(travis);
				}
				keyword = user_variables[keyword];
				for (unsigned int i = 0; i < keyword.size(); ++i) {
					medium.insert(travis, keyword[i]);
				}
				if (travis != medium.begin()) --travis;
				if (travis == medium.end()) break;
			}
		}
		std::string rets;
		for (std::list<char>::iterator finale = medium.begin(); 
			finale != medium.end(); ++finale) {
			rets.push_back(*finale);
		}
		return rets;
	}
	//Interpret some input stream as console input
	void gulp(std::istream& inp) {
		std::string line;
		while (std::getline(inp, line)) {
			if (!line.size())
				continue;
			if (line.front() == ':') {
				line.erase(line.begin());
				digest(line);
			}
			if (line.front() == '"')
				continue;
		}
	}
	//Execute a set of commands from a text file
	bool source(const std::string& filename) {
		std::ifstream prefstream(filename);
		if (prefstream.is_open()) {
			cli::gulp(prefstream);
			prefstream.close();
			return true;
		}
		return false;
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
	std::string reform = var_expand(token);
	std::stringstream input(reform);
	std::string command;
	input >> command;
	//Variable nonsense
	if (command == "var") {
		if (input >> command) {
			std::string value;
			std::string varval;
			while (input >> value) {
				varval += value + ' ';
			}
			user_variables[command] = varval;
			send_message("Variable " + command + " set to " + varval, uSuccess);
			return uSuccess;
		}
	}
	//Create a new binding
	if (command == "map") {
		std::string whomst;
		input >> whomst;
		if (getline(input, command)) {
			bindings[whomst] = command;
			send_message("Key combination " + whomst + " mapped to " + command, uSuccess);
			return uSuccess;
		}
		send_message("Usage is :map <keys> <simulated keystrokes>", uIncorrectUsage);
		return uIncorrectUsage;
	}
	//Force quit
	if (command == "q!") {
		std::string filename(splitPath(currentTab->filepath).second);
		closeTab(currentTab);
		send_message("Closed tab '" + filename + '\'');
		return uSuccess;
	}
	//Quit
	if (command == "q" || command == "quit") {
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
	//Delete the shape currently being worked on
	if (command == "dshape") {
		//This causes a segmentation fault by setting subgraphicshape to currentgraphic()->end()
		currentTab->subGraphicShape = currentTab->currentGraphic().erase(currentTab->subGraphicShape);
		currentTab->makechange();
		if (currentTab->subGraphicShape == currentTab->currentGraphic().end())
			--currentTab->subGraphicShape;
	}
	//Force the mouse to warp within the window
	if (command == "warp" || command == "warpcursor") {
		int X;
		if (input >> X) {
			int Y;
			if (input >> Y) {
				glutWarpPointer(X, Y);
				return uSuccess;
			}
		}
		send_message("Usage is :warp <X> <Y>", uIncorrectUsage);
		return uIncorrectUsage;
	}
	//Force the mouse to click
	if (command == "click") {
		send_message("NOT IMPLEMENTED AHHHH");
		return uError;
	}
	//Write
	if (command == "w" || command == "write") {
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
	//Source
	if (command == "source") {
		//Ensure a filename was provided
		if (!(input >> command)) {
			send_message("Usage is :source <filename>", uIncorrectUsage);
			return uIncorrectUsage;
		}
		else {
			if (source(command))
				return uSuccess;
			else {
				send_message("Error reading file '" + command + '\'', uError);
				return uError;
			}
		}
	}
	//Edit (open)
	if (command == "e" || command == "edit") {
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
	//Clone the current shape to the next layer
	if (command == "cshapen" || command == "cshape") {
		//Make another shape
		currentTab->subGraphicShape = currentTab->currentGraphic().insert(currentTab->subGraphicShape, currentTab->currentShape());
		currentTab->makechange();
		send_message("Cloned shape to next layer");
		return uSuccess;
	}
	//Clone the current shape to the previous layer
	if (command == "cshapep") {
		//Make another shape
		currentTab->subGraphicShape = currentTab->currentGraphic().insert(currentTab->subGraphicShape, currentTab->currentShape());
		currentTab->makechange();
		--currentTab->subGraphicShape;
		send_message("Cloned shape to previous layer");
		return uSuccess;
	}
	//Toggle showing the glyph skeleton
	if (command == "skel" || command == "skeleton") {
		currentTab->show_skeleton = !(currentTab->show_skeleton);
		send_message("show_skeleton set to " + std::to_string(currentTab->show_skeleton));
		return uSuccess;
	}
	//Change the bezier resolution
	if (command == "bezres") {
		unsigned int new_res;
		if (input >> new_res) {
			fgr::BEZIER_RESOLUTION = new_res;
			send_message("Bezier resolution set to " + std::to_string(new_res));
			return uSuccess;
		}
		send_message("Usage is :bezres <int new_resolution>", uIncorrectUsage);
		return uIncorrectUsage;
	}
	//Toggle bezier status of the current glyph
	if (command == "bez" || command == "bezier") {
		currentTab->currentGlyph().bezier = !currentTab->currentGlyph().bezier;
		currentTab->makechange();
		send_message("Current glyph's bezier status set to " + std::to_string(currentTab->currentGlyph().bezier));
		return uSuccess;
	}
	//Change current glyph GL Mode
	if (command == "mode") {
		//Ensure another mode was specified
		if (input >> command) {
			int interpretation = (std::stoi(command) % 11);
			//For now, interpret as plan integer
			currentTab->currentGlyph().mode = static_cast<fgr::GLmode>(interpretation);
			currentTab->makechange();
			send_message("Glyph GL Mode set to " + std::string(currentTab->currentGlyph().glModeString()), uSuccess);
			return uSuccess;
		}
		else {
			send_message("Usage is :mode <GLModeName/GLModeNum>", uIncorrectUsage);
			return uIncorrectUsage;
		}
	}
	//Clear the canvas on the current layer
	if (command == "clear") {
		currentTab->currentGlyph().clear();
		currentTab->makechange();
		return uSuccess;
	}
	//Set the experimental fractal mode depth level
	if (command == "iterations") {
		int newCount;
		if (input >> newCount) {
			currentTab->experimentalFractalIterations = newCount;
			send_message("Set fractal recursion depth to " + std::to_string(newCount));
			return uSuccess;
		}
		else {
			send_message("Usage is :iterations <int recursion-depth>", uIncorrectUsage);
			return uIncorrectUsage;
		}
	}
	if (command == "shapen") {
		++currentTab->subGraphicShape;
		if (currentTab->subGraphicShape == currentTab->currentGraphic().end()) {
			currentTab->subGraphicShape = currentTab->currentGraphic().begin();
			return uSuccess;
		}
	}
	if (command == "nshapen" || command == "nshape") {
		currentTab->subGraphicShape = currentTab->currentGraphic().insert(++currentTab->subGraphicShape, fgr::shape());
		currentTab->makechange();
		return uSuccess;
	}
	if (command == "shapep") {
		if (currentTab->subGraphicShape != currentTab->currentGraphic().begin())
			--currentTab->subGraphicShape;
		return uSuccess;
	}
	if (command == "nshapep") {
		currentTab->subGraphicShape = currentTab->currentGraphic().insert(currentTab->subGraphicShape, fgr::shape());
		currentTab->makechange();
		return uSuccess;
	}
	//Toggle distraction-free mode
	if (command == "zen") {
		//If already in zen mode, exit
		if (zen) {
			glutSetWindowTitle("EXITING ZEN");
			glutPositionWindow(0, 0);
			glutReshapeWindow(900, 900);
			glut32::maximizeWindow("EXITING ZEN");
			currentTab->updateWindowName();
			zen = false;
		}
		//Otherwise, enter zen mode
		else {
			glutFullScreen();
			zen = true;
		}
		return uSuccess;
	}
	//Change current shape color
	if (command == "c" || command == "color") {
		if (currentTab->format != eGlyph) {
			//If more arguments are supplied,
			float newR, newG, newB;
			if (input >> newR && input >> newG && input >> newB) {
				float newA;
				if (input >> newA) {
					currentTab->currentShape().color = fgr::fcolor(newR, newG, newB, newA);
					currentTab->makechange();
					return uSuccess;
				}
				else {
					currentTab->currentShape().color = fgr::fcolor(newR, newG, newB);
					currentTab->makechange();
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
	//Change current shape line thickness
	if ( command == "lw" || command == "linewidth" || command == "thickness") {
		if (currentTab->format != eGlyph) {
			//If more arguments are supplied,
			float newW;
			if (input >> newW) {
				currentTab->currentShape().lineThickness = newW;
				currentTab->makechange();
				return uSuccess;
			}
			//Otherwise, tell them what's what
			else {
				send_message( "Line width - " +
					std::to_string(currentTab->currentShape().lineThickness) + 
					" pixels", uSuccess);
				return uSuccess;
			}
		}
		else {
			send_message("Current art has no member 'line width'", uError);
			return uError;
		}
	}
	//Allow the user to add a vertex using the CLI
	if (command == "v" || command == "vertex") {
		float vX, vY;
		if (input >> vX && input >> vY) {
			currentTab->currentGlyph().push_back(fgr::point(vX, vY));
			currentTab->makechange();
			send_message("Vertex added at " + currentTab->currentGlyph().back().label(), uSuccess);
			return uSuccess;
		}
		else {
			send_message("Usage is :vertex <x-pos> <y-pos>", uIncorrectUsage);
			return uIncorrectUsage;
		}
	}
	//Experimental undo
	if (command == "undo") {
		send_message("We're not there yet.", uError);
		return uError;
	}
	//Change current shape point size
	if ( command == "ps" || command == "pointsize") {
		if (currentTab->format != eGlyph) {
			//If more arguments are supplied,
			float newP;
			if (input >> newP) {
				currentTab->currentShape().pointSize = newP;
				currentTab->makechange();
				send_message("Set point size to " + std::to_string(currentTab->currentShape().pointSize));
				return uSuccess;
			}
			//Otherwise, tell them what's what
			else {
				send_message( "Point size - " + 
					std::to_string(currentTab->currentShape().pointSize) + 
					" pixels", uSuccess);
				return uSuccess;
			}
		}
		else {
			send_message("Current art has no member 'point size'", uError);
			return uError;
		}
	}
	//Change the current tool
	if (command == "t" || command == "tool") {
		short newTool;
		if (input >> newTool) {
			newTool %= tToolCount;
			currentTab->currentTool = (toolNum)newTool;
			send_message("Set tool to " + toolName(currentTab->currentTool), uSuccess);
			return uSuccess;
		}
		else {
			send_message("Currently using tool " + toolName(currentTab->currentTool), uSuccess);
			return uSuccess;
		}
	}
	//Convert the art into another type
	if (command == "convert") {
		//Read in what they would have us convert to
		if (input >> command) {
			editortype target = stringToType(command);
			if (target) {
				currentTab->convertFile(stringToType(command));
				send_message("Converted to file " + currentTab->filepath, uSuccess);
				return uSuccess;
			}
			else {
				send_message("Invalid file type - " + command, uError);
				return uError;
			}
		}
		else {
			send_message("Usage is :convert <target file type>", uIncorrectUsage);
			return uIncorrectUsage;
		}
	}
	//Toggle experimental fractal mode
	if (command == "fractog") {
		currentTab->experimentalFractalMode = !currentTab->experimentalFractalMode;
		send_message("Fractal mode set to " + std::to_string(currentTab->experimentalFractalMode));
		return uSuccess;
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