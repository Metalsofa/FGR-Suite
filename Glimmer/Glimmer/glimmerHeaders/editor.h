/* Defines the 'editor' class */
#pragma once
#ifndef __editor_h__
#define __editor_h__

#include "fgrtoolkit.h"
#include <string> 

//This enumerates the types of editor available
enum editortype { eGlyph, eShape, eGraphic, eAnimation, eSpritesheet };

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
		return x + width * show;
	}
	//Returns the pixel position of the left viewport boundary
	GLint left() const {
		return x ;
	}
	//Returns the pixel position of the top viewport boundary
	GLint top() const {
		return y + height * show;
	}
	//Returns the pixel position of the bottom viewport boundary
	GLint bottom() const {
		return y;
	}
};

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
	glyph* glyphArt;
	//Pointer to the shape this editor has open
	shape* shapeArt;
	//Pointer to the graphic this editor has open
	graphic* graphicArt;
	//Pointer to the animation this editor has open
	animation* animArt;
	// Settings as to whether different editor panes are open, and their sizes when open
	bool showFileTree;
	GLint fileTreeWidth;
	bool showTabHeader;
	GLint tabHeaderHeight;
	bool showAnimationFrames;
	GLint animationFramesWidth;
	bool showLayers;
	GLint layersWidth;
	bool showShapes;
	GLint shapesWidth;
	bool showShapeProperties;
	GLint shapePropertiesHeight;
	GLint shapeColorWidth;
	bool showGlyphGLMode;
	GLint glyphGLModeHeight;
	bool showTools;
	GLint toolsWidth;
	// CONSTRUCTORS
	//Default constructor
	editor() {
		glyphArt = NULL;
		shapeArt = NULL;
		graphicArt = NULL;
		animArt = NULL;
		defaultSettings();
	}
	//Filepath constructor
	editor(const std::string& filepath_) {
		glyphArt = NULL;
		shapeArt = NULL;
		graphicArt = NULL;
		animArt = NULL;
		defaultSettings();
		filepath = filepath_;
		//Open and load the file
	}
	//Editor-type empty graphic constructor
	editor(editortype format_) {
		glyphArt = NULL;
		shapeArt = NULL;
		graphicArt = NULL;
		animArt = NULL;
		defaultSettings();
		configureLayout(format_);
	}
	//Configure the layout of the editor to a particular editor type
	void configureLayout(editortype format_);
	//Set settings to their defaults
	void defaultSettings() {
		showFileTree = true;
		showTabHeader = true;
		showGlyphGLMode = true;
		showTools = true;
		//Default sizes
		fileTreeWidth = 40;
		tabHeaderHeight = 40;
		animationFramesWidth = 40;
		layersWidth = 40;
		shapesWidth = 40;
		shapePropertiesHeight = 40;
		shapeColorWidth = 40;
		glyphGLModeHeight = 40;
		toolsWidth = 40;
	}
	// The following functions are meant to provide information about the layout
	//Right-hand side file-tree display
	viewport superWindowPane() const {
		return viewport(0, 0, GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT);
	}
	viewport fileTreePane() const {
		return viewport(superWindowPane().right() - fileTreeWidth, 0, fileTreeWidth, superWindowPane().height, showFileTree);
	}
	//Top-side editor open tabs display
	viewport tabHeaderPane() const {
		return viewport(0, superWindowPane().top() - tabHeaderHeight, superWindowPane().width, tabHeaderHeight, showTabHeader);
	}
	//Left-hand side animation frames display
	viewport animationFramesPane() const {
		return viewport(0, 0, animationFramesWidth, superWindowPane().height, showAnimationFrames);
	}
	//Left-hand side layers display
	viewport layersPane() const {
		return viewport(0, animationFramesPane().right(), layersWidth, superWindowPane().height, showLayers);
	}
	//Left-hand side shapes display
	viewport shapesPane() const {
		return viewport(0, layersPane().right(), shapesWidth, superWindowPane().height, showShapes);
	}
	//Bottom-side shape-properties display; contains two other panes
	viewport shapePropertiesPane() const {
		return viewport(0, shapesPane().right(), fileTreePane().left() - shapesPane().right(), shapePropertiesHeight, showShapeProperties);
	}
	//Part of the shape-properties display: line thickness and point size
	viewport shapeSpecificationsPane() const {
		return viewport(shapeColorPane().right(), 0, shapePropertiesPane().width - shapeColorPane().width, shapePropertiesPane().height, showShapeProperties);
	}
	//Part of the shape-properties display: color of the current shape
	viewport shapeColorPane() const {
		return viewport(shapePropertiesPane().left(), 0, shapeColorWidth, shapePropertiesPane().height, showShapeProperties);
	}
	//Bottom-side glyph GLmode display
	viewport glyphGLModePane() const {
		return viewport(shapePropertiesPane().left(), shapePropertiesPane().top(), shapePropertiesPane().width, glyphGLModeHeight, showGlyphGLMode);
	}
	//Right-hand side editor tools display
	viewport toolsPane() const {
		return viewport(fileTreePane().left() - toolsWidth, glyphGLModePane().top(), toolsWidth, tabHeaderPane().bottom() - glyphGLModePane().top(), showTools);
	}
};

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
	}
}


#endif
