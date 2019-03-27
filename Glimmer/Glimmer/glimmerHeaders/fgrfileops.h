/* This header file defines file operations for the FGR file format, reading them in a binaries. */
#pragma once

#ifndef __FILEOPS_H__
#define __FILEOPS_H__

#include "fgrtoolkit.h"


//Get a point or set of points from a filestream, and advance the internal position indicator by that many characters.
point fgetpoint(FILE*& stream) {
	//This is the point that will be returned
	float buffer[2];
	fread(buffer, sizeof(float), 2, stream);
	return point(buffer[0], buffer[1]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		GLYPH BINARY STRUCTURE
//
//	| CHAR GLMODE | STD::SIZE_T POINT_COUNT | FLOAT X_1 | FLOAT Y_1 | FLOAT X_2 | FLOAT Y_2 | ... | FLOAT X_N | FLOAT Y_N |
//

//Get a glyph from a file stream
glyph fgetglyph(FILE*& stream) {
	//Read in the GLmode of this glyph
	GLmode GLMODE;
	fread(&GLMODE, sizeof(GLmode), 1, stream);
	//Read in the number of points in the glyph
	std::size_t POINTC;
	fread(&POINTC, sizeof(std::size_t), 1, stream);
	//Read in every point, one at a time
	glyphContainer pointData;
	for (std::size_t i = 0; i < POINTC; ++i) {
		pointData.push_back(fgetpoint(stream));
	}
	return glyph(GLMODE, pointData);
}


/////////////////////////////////////////////////////////////////////////////////
//		FCOLOR BINARY STRUCTURE
//
//	| FLOAT RED | FLOAT GREEN | FLOAT BLUE |
//

//Get a shape from a file stream
fcolor fgetfcolor(FILE*& stream) {
	
}


/////////////////////////////////////////////////////////////////////////////////
//		SHAPE BINARY STRUCTURE
//
//	| FCOLOR COLOR | FLOAT LINETHICKNESS | FLOAT POINTSIZE | <GLYPH> |
//

//Get a shape from a file stream
shape fgetshape(FILE*& stream) {

}


/////////////////////////////////////////////////////////////////////////////////
//		GRAPHIC BINARY STRUCTURE
//
//	| STD::SIZE_T SHAPECOUNT | <SHAPES> |
//

//Get a shape from a file stream
graphic fgetgraphic(FILE*& stream) {

}

///NOTE TO SELF: I'm slightly worried about signed/unsigned binary reading. If you run into problems, be sure to check on that.
/////////////////////////////////////////////////////////////////////////////////
//		FRAME BINARY STRUCTURE
//
//	| INT DELAY | <GRAPHIC> |
//

//Get a shape from a file stream
frame fgetframe(FILE*& stream) {

}


/////////////////////////////////////////////////////////////////////////////////
//		FRAME BINARY STRUCTURE
//
//	| INT DELAY | <GRAPHIC> |
//

//Get a shape from a file stream
animation fgetanimation(FILE*& stream) {

}



#endif
