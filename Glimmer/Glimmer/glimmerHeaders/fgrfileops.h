/* This header file defines file operations for the FGR file format, reading them in a binaries. */
#pragma once

#ifndef __FILEOPS_H__
#define __FILEOPS_H__

#include "fgrtoolkit.h"



////////////////////////////////////////////////////////////////////////
//		POINT BINARY STRUCTURE
//
//	| FLOAT X | FLOAT Y |
//

//Get a point or set of points from a filestream, and advance the internal position indicator by that many characters.
point fgetpoint(FILE*& stream) {
	//This is the point that will be returned
	float buffer[2];
	fread(buffer, sizeof(float), 2, stream);
	return point(buffer[0], buffer[1]);
}


////////////////////////////////////////////////////////////////////////
//		ORDERED CONTAINER CLASS BINARY STRUCTURE
//
//	| STD::SIZE_T OBJECTCOUNT | <OBJECTS> |
//

//This function is not yet properly implemented

/* Get a vector, list, etc. of class instances from a filestream. A function
must be specified for reading in an individual instance of said class */
template <class entry, class container>
container fgetcontainer(FILE*& stream, entry(*fgetfunc)(FILE*&)) {
	//Load in the object count
	std::size_t objc;
	fread(&objc, sizeof(std::size_t), 1, stream);
	//Load in the container and fill it up
	container retc;
	for (std::size_t i = 0; i < objc; ++i) {
		retc.push_back(fgetfunc(stream));
	}
	return retc;
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
	//Construct and return the glyph object
	return glyph(GLMODE, pointData);
}


//////////////////////////////////////////////////////////
//		FCOLOR BINARY STRUCTURE
//
//	| FLOAT RED | FLOAT GREEN | FLOAT BLUE | FLOAT ALPHA |
//

//Get a shape from a file stream
fcolor fgetfcolor(FILE*& stream) {
	//Read in the colors
	float input [4];
	fread(input, sizeof(float), 4, stream);
	//Construct and return the fcolor
	return fcolor(input[0], input[1], input[2], input[3]);
}


//////////////////////////////////////////////////////////////////////
//		SHAPE BINARY STRUCTURE
//
//	| FCOLOR COLOR | FLOAT LINETHICKNESS | FLOAT POINTSIZE | <GLYPH> |
//

//Get a shape from a file stream
shape fgetshape(FILE*& stream) {
	//Load in the fcolor
	fcolor color = fgetfcolor(stream);
	//Load in the linethickness and pointsize
	float weightdata [2];
	fread(weightdata, sizeof(float), 2, stream);
	//Load in the point data
	glyph pointdata = fgetglyph(stream);
	//Construct and return the shape
	return shape(pointdata, color, weightdata[0], weightdata[1]);
}


/////////////////////////////////////////
//		GRAPHIC BINARY STRUCTURE
//
//	| STD::SIZE_T SHAPECOUNT | <SHAPES> |
//

//Get a shape from a file stream
graphic fgetgraphic(FILE*& stream) {
	//Load in the size type
	std::size_t shapecount;
	fread(&shapecount, sizeof(std::size_t), 1, stream);
	//Load in the shapes
	graphicContainer shapedata;
	for (std::size_t i = 0; i < shapecount; ++i) {
		shapedata.push_back(fgetshape(stream));
	}
	//Construct and return the graphic object
	return graphic(shapedata);
}

///NOTE TO SELF: I'm slightly worried about signed/unsigned binary reading. If you run into problems, be sure to check on that.
/////////////////////////////////////////////////////////////////////////////////
//		FRAME BINARY STRUCTURE
//
//	| INT DELAY | <GRAPHIC> |
//

//Get a frame of an animtion from a file stream
frame fgetframe(FILE*& stream) {
	//Load in the delay
	int delay;
	fread(&delay, sizeof(int), 1, stream);
	//Load in the graphic, construct and return the frame
	return frame(delay, fgetgraphic(stream));
}


/////////////////////////////////////////////////////////////////////////////////
//		FRAME BINARY STRUCTURE
//
//	| INT DELAY | <GRAPHIC> |
//

//Get an animation from a file stream
animation fgetanimation(FILE*& stream) {
	//The animation class isn't complete yet
}



#endif
