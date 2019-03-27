/* Defines the 'editor' class */
#pragma once
#ifndef __editor_h__
#define __editor_h__

#include "fgrtoolkit.h"
#include <string> 

//This enumerates the types of editor available
enum editortype { eGlyph, eShape, eGraphic, eAnimation, eSpritesheet };

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
	// CONSTRUCTORS
	//Default constructor
	editor() {
		glyphArt = NULL;
		shapeArt = NULL;
		graphicArt = NULL;
		animArt = NULL;
	}
	// Filepath constructor
	editor(const std::string& filepath_) {
		glyphArt = NULL;
		shapeArt = NULL;
		graphicArt = NULL;
		animArt = NULL;
		filepath = filepath_;
	}
};



#endif
