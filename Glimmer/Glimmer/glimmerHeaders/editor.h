/* Defines the 'editor' class which can be thought of as a 'tab' */
#pragma once
#ifndef __editor_h__
#define __editor_h__

#include "fgrutils.h"

#include <string> 
#include <utility>

//This enumerates the types of editor available
enum editortype { eNULL, eGlyph, eShape, eGraphic, eAnimation, eSpritesheet };

//Returns just the file extention of a std::string
std::string getExtention(const std::string& filename) {
	std::size_t i;
	for (i = filename.size() - 1; i > 0; --i) {
		if (filename[i] == '.' || filename[i] == '\\' || filename[i] == '/')
			break;
	}
	std::string ext;
	if ( (i == 0 && !(filename[0] == '.')) || filename[i] == '\\' || filename[i] == '/')
		return ext;
	for (++i; i < filename.size(); ++i) {
		ext.push_back(filename[i]);
	}
	return ext;
}

//Returns a pair of std::strings where the first is the relative path, 
//and the second is the filename (including the extention)
std::pair<std::string, std::string> splitPath(const std::string& path) {
	std::pair<std::string, std::string> retp;
	if (!path.size()) {
		return retp;
	}
	//Iterate backwards through the std::string
	std::size_t i;
	for (i = path.size() - 1;  i && path[i] != '\\' && path[i] != '/'; --i);
	retp.first = path.substr(0, i);
	retp.second = path.substr(i, path.size() - i);
	return retp;
}

//Returns an editor type by interpreting a std::string as a file extention
editortype interpretextention(const std::string& ext) {
	if (ext == "fgl")
		return eGlyph;
	if (ext == "fsh")
		return eShape;
	if (ext == "fgr")
		return eGraphic;
	if (ext == "fan")
		return eAnimation;
	if (ext == "fss")
		return eSpritesheet;
	return eNULL;
}

//Returns the three-letter file extension associated with a type of fourier graphic. Period not included.
std::string associatedExtention(editortype format) {
	switch (format) {
	case eGlyph:
		return "fgl";
	case eShape:
		return "fsh";
	case eGraphic:
		return "fgr";
	case eAnimation:
		return "fan";
	case eSpritesheet:
		return "fss";
	default:
		return "dat";
	}
}

//Stores viewport specifications
class viewport {
public:
	//Representation
	GLint x;
	GLint y;
	GLsizei width;
	GLsizei height;
	//Won't always matter; but represents whether it should be shown
	bool show;
	//Default constructor
	viewport() {
		x = 0;
		y = 0;
		width = 10;
		height = 10;
		show = true;
	}
	//Know-it-all constructor
	viewport(GLint x_, GLint y_, GLsizei width_, GLsizei height_, bool show_) {
		x = x_;
		y = y_;
		width = width_;
		height = height_;
		show = show_;
	}
	//Know-Most and show-it-all constructor
	viewport(GLint x_, GLint y_, GLsizei width_, GLsizei height_) {
		x = x_;
		y = y_;
		width = width_;
		height = height_;
		show = true;
	}
	// Returns the pixel position of one of the viewport's four boundaries
	//Returns the pixel position of the right viewport boundary
	GLint right() const {
		return x + width * !!show;
	}
	//Returns the pixel position of the left viewport boundary
	GLint left() const {
		return x ;
	}
	//Returns the pixel position of the top viewport boundary
	GLint top() const {
		return y + height * !!show;
	}
	//Returns the pixel position of the bottom viewport boundary
	GLint bottom() const {
		return y;
	}
};

viewport superWindowPane() {
	return viewport(0, 0, windowWidth, windowHeight);
}

//Think of these like tabs
class editor {
public:
	// REPRESENTATION
	// What type of editor this is
	editortype format;
	// Absolute file path to the file this editor is concerned with
	std::string filepath;
	// Pointers to the art being worked on.
	//Pointer to the glyph this editor has open
	fgr::glyph* glyphArt;
	//Pointer to the shape this editor has open
	fgr::shape* shapeArt;
	//Pointer to the graphic this editor has open
	fgr::graphic* graphicArt;
	//Pointer to the animation this editor has open
	fgr::animation* animArt;
	//Whether or not there are unsaved changes
	bool unsavedChanges;
	//True iff this is a blank graphic nobody is going to miss.
	bool blankFile;
	// CENTRAL EDITOR PANE SETTINGS
	fgr::point pan;
	float zoom;
	float rotation;
	// Settings as to whether different editor panes are open, and their sizes when open
	//COMMAND LINE (BOTTOM)
		bool showCommandLine;
		GLint commandLineHeight;
		fgr::fcolor commandLineColor;
	//FILE TREE (RIGHT)
		bool showFileTree;
		GLint fileTreeWidth;
		fgr::fcolor fileTreeColor;
	//TAB HEADER (TOP)
		bool showTabHeader;
		GLint tabHeaderHeight;
		fgr::fcolor tabHeaderColor;
	//ANIMATION FRAMES (LEFT)
		bool showAnimationFrames;
		GLint animationFramesWidth;
		fgr::fcolor animationFramesColor;
	//LAYERS (LEFT)
		bool showLayers;
		GLint layersWidth;
		fgr::fcolor layersColor;
	//SHAPES (LEFT)
		bool showShapes;
		GLint shapesWidth;
		fgr::fcolor shapesColor;
	//SHAPE PROPERTIES (BOTTOM)
		bool showShapeProperties;
		GLint shapePropertiesHeight;
		GLint shapeColorWidth;
		fgr::fcolor shapeColorColor;
		fgr::fcolor shapeSpecificationsColor;
	//GLYPH GLMODE (BOTTOM)
		bool showGlyphGLMode;
		GLint glyphGLModeHeight;
		fgr::fcolor glyphGLModeColor;
	//TOOLS (RIGHT)
		bool showTools;
		GLint toolsWidth;
		fgr::fcolor toolsColor;
	// CONSTRUCTORS
	//Default constructor
	editor() {
		glyphArt = NULL;
		shapeArt = NULL;
		graphicArt = NULL;
		animArt = NULL;
		unsavedChanges = true;
		defaultSettings();
	}
	//Filepath constructor
	editor(const std::string& filepath_) {
		glyphArt =		NULL;
		shapeArt =		NULL;
		graphicArt =	NULL;
		animArt =		NULL;
		defaultSettings();
		filepath = filepath_;
		unsavedChanges = false;
		//Open and load the file
		loadFile(filepath_);
	}
	//Editor-type empty graphic constructor
	editor(editortype format_) {
		glyphArt =		NULL;
		shapeArt =		NULL;
		graphicArt =	NULL;
		animArt =		NULL;
		defaultSettings();
		configureLayout(format_);
		newfile(format_);
		unsavedChanges = true;
	}
	//Copy constructor
	editor(const editor& other) {
		//Everything except these pointers can be copied by value directly
		format = other.format;
		filepath = other.filepath;
		unsavedChanges = other.unsavedChanges;
		glyphArt =		NULL;
		shapeArt =		NULL;
		graphicArt =	NULL;
		animArt =		NULL;
		if (other.glyphArt)
			glyphArt = new fgr::glyph(*other.glyphArt);
		if (other.shapeArt)
			shapeArt = new fgr::shape(*other.shapeArt);
		if (other.graphicArt)
			graphicArt = new fgr::graphic(*other.graphicArt);
		if (other.animArt)
			animArt = new fgr::animation(*other.animArt);
		//  LATER THIS HAS TO BE EXPANDED.
		defaultSettings();
	}
	//Returns false if the extention is not recognized or the file contains a segmentation problem
	bool loadFile(const std::string& filepath);
	//Save the current artwork into a file
	bool save();
	//Save the current artwork to the specified file path
	bool save(const std::string& path);
	//Be careful - this function does not save any progress first
	void deleteAllArt();
	//Configure the layout of the editor to a particular editor type
	void configureLayout(editortype format_);
	//Change the name of the current window to reflect this editor
	void updateWindowName() const;
	//Update the text on the tab header
	std::string tabTitle() const;
	//Get the name of the file being worked on
	std::string getFileName() const {
		return splitPath(filepath).second;
	}
	//Set settings to their defaults
	void defaultSettings() {
		//Default editor pane settings
		pan = fgr::point();
		zoom = 0;
		rotation = 0;
		//Default booleans
		showFileTree =				true;
		showTabHeader =				true;
		showGlyphGLMode =			true;
		showTools =					true;
		showCommandLine =			true;
		//Default colors
		commandLineColor =			fgr::fcolor(0.0f, 0.0f, 0.0f);
		fileTreeColor =				fgr::fcolor(0.2f, 0.2f, 0.2f);
		tabHeaderColor =			fgr::fcolor(0.7f, 0.7f, 0.7f);
		animationFramesColor =		fgr::fcolor(0.3f, 0.3f, 0.3f);
		layersColor =				fgr::fcolor(0.4f, 0.4f, 0.4f);
		shapesColor =				fgr::fcolor(0.3f, 0.3f, 0.3f);
		shapeColorColor =			fgr::fcolor(0.4f, 0.4f, 0.4f);
		shapeSpecificationsColor =	fgr::fcolor(0.4f, 0.4f, 0.4f);
		glyphGLModeColor =			fgr::fcolor(0.6f, 0.6f, 0.6f);
		toolsColor =				fgr::fcolor(0.3f, 0.3f, 0.3f);
		//Default sizes
		commandLineHeight =			24;
		fileTreeWidth =				200;
		tabHeaderHeight =			50;
		animationFramesWidth =		100;
		layersWidth =				100;
		shapesWidth =				100;
		shapePropertiesHeight =		100;
		shapeColorWidth =			200;
		glyphGLModeHeight =			commandLineHeight;
		toolsWidth =				100;
	}
	// The following functions are meant to provide information about the layout
	//Bottom-side command line interface
	viewport commandLinePane() const {
		return viewport(superWindowPane().left(), superWindowPane().bottom(), superWindowPane().right(), commandLineHeight, showCommandLine);
	}
	//Right-hand side file-tree display
	viewport fileTreePane() const {
		return viewport(superWindowPane().right() - fileTreeWidth, commandLinePane().top(), fileTreeWidth, superWindowPane().top() - commandLinePane().top(), showFileTree);
	}
	//Top-side editor open tabs display
	viewport tabHeaderPane() const {
		return viewport(superWindowPane().left(), superWindowPane().top() - tabHeaderHeight, fileTreePane().left(), tabHeaderHeight, showTabHeader);
	}
	//Left-hand side animation frames display
	viewport animationFramesPane() const {
		return viewport(superWindowPane().left(), commandLinePane().top(), animationFramesWidth, tabHeaderPane().bottom() - commandLinePane().top(), showAnimationFrames);
	}
	//Left-hand side layers display
	viewport layersPane() const {
		return viewport(animationFramesPane().right(), commandLinePane().top(), layersWidth, animationFramesPane().top() - commandLinePane().top(), showLayers);
	}
	//Left-hand side shapes display
	viewport shapesPane() const {
		return viewport(layersPane().right(), commandLinePane().top(), shapesWidth, animationFramesPane().top() - commandLinePane().top(), showShapes);
	}
	//Bottom-side shape-properties display; contains two other panes
	viewport shapePropertiesPane() const {
		return viewport(shapesPane().right(), commandLinePane().top(), fileTreePane().left() - shapesPane().right(), shapePropertiesHeight, showShapeProperties);
	}
	//Part of the shape-properties display: line thickness and point size
	viewport shapeSpecificationsPane() const {
		return viewport(shapeColorPane().right(), commandLinePane().top(), shapePropertiesPane().width - shapeColorPane().width, shapePropertiesPane().height, showShapeProperties);
	}
	//Part of the shape-properties display: color of the current shape
	viewport shapeColorPane() const {
		return viewport(shapePropertiesPane().left(), commandLinePane().top(), shapePropertiesPane().width / 2, shapePropertiesPane().height, showShapeProperties);
	}
	//Bottom-side glyph GLmode display
	viewport glyphGLModePane() const {
		return viewport(shapePropertiesPane().left(), shapePropertiesPane().top(), shapePropertiesPane().width, glyphGLModeHeight, showGlyphGLMode);
	}
	//Right-hand side editor tools display
	viewport toolsPane() const {
		return viewport(fileTreePane().left() - toolsWidth, glyphGLModePane().top(), toolsWidth, tabHeaderPane().bottom() - glyphGLModePane().top(), showTools);
	}
	//Central editor display
	viewport centralPane() const {
		return viewport(shapesPane().right(), glyphGLModePane().top(), toolsPane().left() - shapesPane().right(), tabHeaderPane().bottom() - glyphGLModePane().top());
	}
	// DESTRUCTOR
	//Deletes all art before going out of scope
	~editor() {
		deleteAllArt();
	}
	// OTHER METHODS
	//Load an empty file of a given kind, which will cause loss of unsaved changes
	void newfile(editortype filetype) {
		deleteAllArt();
		configureLayout(filetype);
		filepath = "untitled." + associatedExtention(filetype);
		unsavedChanges = true;
		switch (filetype) {
		case eAnimation:
			animArt = new fgr::animation;
			break;
		default:
		case eGraphic:
			graphicArt = new fgr::graphic;
			break;
		case eShape:
			shapeArt = new fgr::shape;
			break;
		case eGlyph:
			glyphArt = new fgr::glyph;
			break;
		}
	}
	// EDITING FUNCTIONALITIES
	//Editing to the selected shape/glyph should always be done through this
	fgr::glyph& currentGlyph() const;
};

/* Editing to the selected shape/glyph should always be done through this.
 * Be sure to update the 'blankFile' flag to false if something is changed!
 * As well as the 'unsavedChanges' flag!!!! */
fgr::glyph& editor::currentGlyph() const {
	switch (format) {
	case eSpritesheet:
		//NOT YET SUPPORTED
		break;
	case eAnimation:
		//NOT YET SUPPORTED
		break;
	case eGraphic:
		//NOT YET SUPPORTED
		break;
	case eShape:
		return *shapeArt;
	case eGlyph:
		return *glyphArt;
	}
	//Exception
	assert(0 && "Bad logic in editor::currentGlyph");
	return *glyphArt;
}

//Correctly format the editor layout to a particular editor type as is neccessary
void editor::configureLayout(editortype format_) {
	//Set the format variable
	format = format_;
	//Correctly set up the layout based on the format
	switch (format) {
		//Format the editor for arranging and editing a spritesheet
	case eSpritesheet:

		break;
		//Format the editor for working on a full animation
	case eAnimation:
		showAnimationFrames = true;
		//Format the editor for working on a complete graphic
	case eGraphic:
		showLayers = true;
		showShapes = true;
		//Format the editor to handle working on a single shape
	case eShape:
		showShapeProperties = true;
		//Format the editor to handle no more than a single glyph
	case eGlyph:
		showGlyphGLMode = true;
		break;
	case eNULL:
		//Error handling
		break;
	}
}

//Set the text shown on the tab header
std::string editor::tabTitle() const {
	std::string rets(getFileName());
	if (unsavedChanges) rets += "[+]";
	rets += " - ";
	switch (format) {
	case eSpritesheet:
		rets += "Fourier Spritesheet";
		break;
	case eAnimation:
		rets += "Fourier Animation";
		break;
	case eGraphic:
		rets += "Fourier Graphic";
		break;
	case eShape:
		rets += "Fourier Shape";
		break;
	case eGlyph:
		rets += "Fourier Glyph";
		break;
	default:
		rets += "Fourier <INVALID_EDITOR_TYPE>";
	}
	rets += " Editor";
	return rets;
}

//Set the name of the GLUT window to reflect this editor
void editor::updateWindowName() const {
	std::string title = "Glimmer - ";
	title += getFileName();
	if (unsavedChanges)
		title += "[+]";
	glutSetWindowTitle(title.c_str());
}

//Be very careful - this function does not save any progress first. It's primary purpose
//is to prevent memory leaks
void editor::deleteAllArt() {
	//How this is handled depends on the editor type
	switch (format) {
	case eAnimation:
		if (animArt)
			delete animArt;
		animArt = NULL;
	case eGraphic:
		if (graphicArt)
			delete graphicArt;
		graphicArt = NULL;
	case eShape:
		if (shapeArt)
			delete shapeArt;
		shapeArt = NULL;
	case eGlyph:
		if (glyphArt)
			delete glyphArt;
		glyphArt = NULL;
	}
	unsavedChanges = false;
	assert(!(glyphArt || shapeArt || graphicArt || animArt) && "All art deleted from heap.");
	return;
}

//Write the current artwork to a particular file path
bool editor::save(const std::string& path) {
	editortype targetFiletype = interpretextention(getExtention(path));
	filepath = path;
	//If the target extention is different from the current one,
	if (format != targetFiletype) {
		//Fail if the extention is invalid
		if (!targetFiletype)
			return false;
		/*For now, also fail in any other mismatch case.
		 *This function should not be the one to convert this graphic
		 *into another type. The CLI should be using a different
		 *function to convert the file format.*/
		return false;
	}
	blankFile = false;
	switch (format) {
	case eGlyph:
		return fgr::glyphToFile(*glyphArt, path);
	case eShape:
		return fgr::shapeToFile(*shapeArt, path);
	case eGraphic:
		return fgr::graphicToFile(*graphicArt, path);
	case eAnimation:
		return fgr::animationToFile(*animArt, path);
	case eSpritesheet:

		return false;
	//Otherwise we're dealing with enull
	default:
		return false;
	}
}

//Write the current artwork to the current file path
bool editor::save() {
	return save(filepath);
}

//Returns false if the extention is not recognized or the file contains a segmentation problem
//Be careful - this will cause any unsaved changes to be lost.
bool editor::loadFile(const std::string& path) {
	//Figure out what format this editor should be from the file extention
	editortype artform = interpretextention(getExtention(path));
	//Error-check the provided file extention
	if (!artform)
		return false;
	//Delete all art from the heap before changing formats
	deleteAllArt();
	//Update the file path
	filepath = path;
	//Reconfigure the layout (also sets the editor format)
	configureLayout(artform);
	//Update the format
	format = artform;
	//Load the appropriate graphic type onto the heap and point to it
	bool foundFile = true;
	switch (artform) {
	case eGlyph:
		glyphArt = new fgr::glyph;
		return fgr::glyphFromFile(*glyphArt, path);
	case eShape:
		shapeArt = new fgr::shape;
		return fgr::shapeFromFile(*shapeArt, path);
	case eGraphic:
		graphicArt = new fgr::graphic;
		return fgr::graphicFromFile(*graphicArt, path);
	case eAnimation:
		animArt = new fgr::animation;
		return fgr::animationFromFile(*animArt, path);
	case eSpritesheet:

		break;
	//Otherwise we're dealing with enull
	default:
		return false;
	}
	//Set the name of the window appropriately
	filepath = path;
	updateWindowName();
	return true;
}

//Draw a quadrilateral the size of the screen onto the screen
void fullScreenQuad() {
	//GLint viewPortDims[4];
	//glGetIntegerv(GL_VIEWPORT, viewPortDims);
	glBegin(GL_QUADS);
		////Bottom left
		//glVertex2i(viewPortDims[0], viewPortDims[1]);
		////Bottom right
		//glVertex2i(viewPortDims[2], viewPortDims[1]);
		////Top right
		//glVertex2i(viewPortDims[2], viewPortDims[3]);
		////Top left
		//glVertex2i(viewPortDims[0], viewPortDims[3]);
		glVertex2i(0, 0); glVertex2i(windowWidth, 0); glVertex2i(windowWidth, windowHeight); glVertex2i(0, windowHeight);
	glEnd();
}

//Draw the outline of a viewport
void outlineViewport(const viewport& pane) {
	glBegin(GL_LINE_LOOP);
		glVertex2i(pane.left(), pane.bottom());
		glVertex2i(pane.left(), pane.top());
		glVertex2i(pane.right(), pane.top());
		glVertex2i(pane.right(), pane.bottom());
	glEnd();
}

//Set the GL-Viewport using an instance of a viewport class.
void setViewport(const viewport& rectangle, bool fillscreen = true) {
	glViewport(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glRasterPos2i(rectangle.left(), rectangle.top() - 6);
	glOrtho(rectangle.left(), rectangle.right(), rectangle.bottom(), rectangle.top(), -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (fillscreen) {
		fullScreenQuad();
		glColor3f(1.0f, 1.0f, 1.0f);
		outlineViewport(rectangle);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(rectangle.left() + 4, rectangle.top() - (18 - 1));
}

//Draw render an editor using OpenGL instructions
void drawEditor(const editor& workbench) {
	void* fontNum = GLUT_BITMAP_HELVETICA_18;
	//Draw the File Tree
	if (workbench.showFileTree) {
		fgr::setcolor(workbench.fileTreeColor);
		setViewport(workbench.fileTreePane());
		if (sessionFilePath.size()) {
			for (char c : sessionFilePath)
				glutBitmapCharacter(fontNum, c);
		}
		else {
			for (char c : "[NO SESSION DIRECTORY]")
				glutBitmapCharacter(fontNum, c);
		}
	}
	//Draw the Tab Header
	if (workbench.showTabHeader) {
		fgr::setcolor(workbench.tabHeaderColor);
		setViewport(workbench.tabHeaderPane());
		std::string text(workbench.tabTitle());
		for (char c : text)
			glutBitmapCharacter(fontNum, c);
	}
	//Draw the Animation Frames
	if (workbench.showAnimationFrames) {
		fgr::setcolor(workbench.animationFramesColor);
		setViewport(workbench.animationFramesPane());
		for (char c : "<Animation Frames>")
			glutBitmapCharacter(fontNum, c);
	}
	//Draw the Layers
	if (workbench.showLayers) {
		fgr::setcolor(workbench.layersColor);
		setViewport(workbench.layersPane());
		for (char c : "<Layers>")
			glutBitmapCharacter(fontNum, c);
	}
	//Draw the Shapes
	if (workbench.showShapes) {
		fgr::setcolor(workbench.shapesColor);
		setViewport(workbench.shapesPane());
		for (char c : "<Shapes>")
			glutBitmapCharacter(fontNum, c);
	}
	//Draw the Shape Properties
	if (workbench.showShapeProperties) {
		//Shape color
		fgr::setcolor(workbench.shapeColorColor);
		setViewport(workbench.shapeColorPane());
		for (char c : "<Shape Color>")
			glutBitmapCharacter(fontNum, c);
		//Shape specifications
		fgr::setcolor(workbench.shapeSpecificationsColor);
		setViewport(workbench.shapeSpecificationsPane());
		for (char c : "<Shape Specifications>")
			glutBitmapCharacter(fontNum, c);
	}
	//Draw the Glyph GLmode
	if (workbench.showGlyphGLMode) {
		fgr::setcolor(workbench.glyphGLModeColor);
		setViewport(workbench.glyphGLModePane());
		std::string modeword;
		if (workbench.glyphArt)
			modeword = workbench.glyphArt->glModeString();
		else
			modeword = "No glyph loaded";
		for (char c : modeword)
			glutBitmapCharacter(fontNum, c);
	}
	//Draw the Tools
	if (workbench.showTools) {
		fgr::setcolor(workbench.toolsColor);
		setViewport(workbench.toolsPane());
		for (char c : "<Tools>")
			glutBitmapCharacter(fontNum, c);
	}
	//Draw the central editor
	if (true) {
		setViewport(workbench.centralPane(), false);
		outlineViewport(workbench.centralPane());
		std::string label("Editing glyph at <address> - " + std::to_string(0) + " bytes");
		for (char c : label)
			glutBitmapCharacter(fontNum, c);
		glTranslatef(workbench.pan.x(), workbench.pan.y(), 0.0f);
		glScalef(workbench.zoom, workbench.zoom, workbench.zoom);
		glRotatef(workbench.rotation, 0.0f, 0.0f, 1.0f);
		switch (workbench.format)
		{
		case eAnimation:
			fgr::draw(*workbench.animArt);
			break;
		case eGraphic:
			fgr::draw(*workbench.graphicArt);
			break;
		case eShape:
			fgr::draw(*workbench.shapeArt);
			break;
		case eGlyph:
			fgr::draw(*workbench.glyphArt);
			break;
		default:
			assert(0 && "INVALID GRAPHIC TYPE");
		}

	}
}


#endif
