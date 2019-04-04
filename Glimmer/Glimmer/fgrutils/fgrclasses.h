/*The purpose of this header file is to implement methods associated with
user-friendly fgr manipulation*/

#pragma once

#ifndef __FGR_TOOLKIT_h__
#define __FGR_TOOLKIT_h__

#include "fgrgeometry.h"
#include "fgrcolor.h"

#include <string>
#include <vector>
#include <list>
#include <map>
#include <cassert>
#include <iostream>

namespace fgr {
	// Enumerate glModes to make it easy to remember
	enum GLmode {
		glPoints, glLines, glLineLoop, glLineStrip, glTriangles,
		glTriangleStrip, glTriangleFan, glQuads, glQuadStrip, glPolygon
	};

	//Forward=declarations
	class glyph;
	class shape;
	class graphic;
	class frame;
	class animation;
	class spritesheet;
	//Container typedefs
	typedef std::list<point> glyphContainer;
	typedef std::vector<shape> graphicContainer;
	typedef std::vector<frame> animationContainer;

	// The most basic FGR type, has only form.
	class glyph : public glyphContainer {
	public:
		using glyphContainer::iterator;
		using glyphContainer::push_back;
		using glyphContainer::erase;
		using glyphContainer::insert;
		using glyphContainer::size;
		using glyphContainer::begin;
		using glyphContainer::end;
		// The GLmode by which this glyph should be rendered
		GLmode mode;
		//Default constructor
		glyph() : glyphContainer() {
			mode = glPoints;
		}
		//Copy constructor
		glyph(const glyph& other) : glyphContainer(other) {
			mode = other.mode;
		}
		//Know-it-all constructor
		glyph(GLmode drawingMode, glyphContainer pointData) : glyphContainer(pointData) {
			mode = drawingMode;
		}
		// Inefficient, but just in case, allow indexing.
		point& operator[] (std::size_t where) {
			// Error check for out-of-bounds indexing
			assert(where < size());
			// Iterate to the right place;
			iterator itr = begin();
			for (std::size_t i = 0; i < where; ++itr, ++i);
			return *itr;
		}
		// OPERATORS
		// Translate this glyph positively by a certain degree
		void operator+= (const point& org) {
			for (iterator itr = begin(); itr != end(); itr++) {
				*itr += org;
			}
		}
		// Translate this glyph negatively, ie. set the origin
		void operator-= (const point& org) {
			for (iterator itr = begin(); itr != end(); itr++) {
				*itr -= org;
			}
		}
		// Scale by a different degree in each dimension
		void operator*= (const point& scalingFactor) {
			for (iterator itr = begin(); itr != end(); itr++) {
				itr->xmult(scalingFactor.x());
				itr->ymult(scalingFactor.y());
			}
		}
		// Scale while maintaining the aspect ratio
		void operator*= (float scalingFactor) {
			for (iterator itr = begin(); itr != end(); itr++) {
				*itr *= scalingFactor;
			}
		}
		//Translate every vertex relative to the origin
		void setOrigin(point org) {
			for (iterator itr = begin(); itr != end(); itr++) {
				*itr -= org;
			}
		}
		//Scale this shape around the origin
		void rescale(float scalingFactor) {
			*this *= scalingFactor;
		}
		//Apply a function to every point in the list
		void applyToAll(void(*transformFunc)(point&)) {
			for (iterator itr = begin(); itr != end(); ++itr) {
				transformFunc(*itr);
			}
		}
		//Apply a function to every point in the list
		void applyToAll(void(*transformFunc)(const point&)) const {
			for (const_iterator itr = begin(); itr != end(); ++itr) {
				transformFunc(*itr);
			}
		}
	};

	// More commonly used than the glyph, the shape has color and a few other aspects
	class shape : public glyph {
	public:
		using glyph::iterator;
		using glyph::push_back;
		using glyph::erase;
		using glyph::insert;
		using glyph::size;
		using glyph::begin;
		using glyph::end;
		// REPRESENTATION
		fgr::fcolor color;
		float lineThickness;
		float pointSize;
		//Default constructor
		shape() : glyph() {
			color = fgr::fcolor();
			lineThickness = 1.0f;
			pointSize = 1.0f;
		}
		//Copy constructor
		shape(const shape& other) : glyph(other) {
			color = other.color;
			lineThickness = other.lineThickness;
			pointSize = other.pointSize;
		}
		//Construct from glyph
		shape(glyph pointData) : glyph(pointData) {
			color = fgr::fcolor();
			lineThickness = 1.0f;
			pointSize = 1.0f;
		}
		//Construct from a glyph, color, line thickness informaiton, and pointsize information
		shape(glyph pointData, fgr::fcolor color_, float lineWidth_, float pointSize_) : glyph(pointData) {
			color = color_;
			lineThickness = lineWidth_;
			pointSize = pointSize_;
		}
		//Scale this shape strictly in the x-dimension around the origin
		void rescaleX(float scalingFactor) {
			for (iterator itr = begin(); itr != end(); itr++) {
				itr->xmult(scalingFactor);
			}
		}
		//Scale this shape strictly in the y-dimension around the origin
		void rescaleY(float scalingFactor) {
			for (iterator itr = begin(); itr != end(); itr++) {
				itr->ymult(scalingFactor);
			}
		}
		//Returns the diagonally-spanning segment for this shape
		const segment bounds() const {
			segment rets = segment(front(), *(++begin()));
			//Iterate through every point and set the bounding segment accordingly
			for (const_iterator itr = begin(); itr != end(); itr++) {
				//Check rightbounds
				if (itr->x() > rets.p2.x()) {
					rets.p2.x(itr->x());
				}
				//Check topbounds
				if (itr->y() > rets.p2.y()) {
					rets.p2.y(itr->y());
				}
				//Check leftbounds
				if (itr->x() < rets.p1.x()) {
					rets.p1.x(itr->x());
				}
				//Check bottombounds
				if (itr->y() < rets.p1.y()) {
					rets.p1.y(itr->y());
				}
			}
			//Now rets is the bounding segment for this shape!
			return rets;
		}
		//Returns the name of the GL drawing mode associated with this shape
		std::string getGLMODE() const {
			switch (mode % 10) {
			case 0:
				return "GL_POINTS";
			case 1:
				return "GL_LINES";
			case 2:
				return "GL_LINE_LOOP";
			case 3:
				return "GL_LINE_STRIP";
			case 4:
				return "GL_TRIANGLES";
			case 5:
				return "GL_TRIANGLE_STRIP";
			case 6:
				return "GL_TRIANGLE_FAN";
			case 7:
				return "GL_QUADS";
			case 8:
				return "GL_QUAD_STRIP";
			case 9:
				return "GL_POLYGON";
			}
			std::cerr << "Something happened in players.h/shape/getGLMODE that isn't supposed to..." << std::endl;
			return "ERR_NOT_GLMODE";
		}
	};

	//Unlike a shape, a graphic has multiple layers
	class graphic : public graphicContainer {
	public:
		using graphicContainer::iterator;
		using graphicContainer::push_back;
		using graphicContainer::insert;
		using graphicContainer::erase;
		using graphicContainer::size;
		using graphicContainer::begin;
		using graphicContainer::end;
		//Apply a function to every shape in the graphic
		void applyToAll(void(*transformFunc)(shape&)) {
			for (iterator itr = begin(); itr != end(); ++itr) {
				transformFunc(*itr);
			}
		}
		//Apply a function to every point in every shape in the graphic
		void applyToAll(void(*transformFunc)(point&)) {
			for (iterator itr = begin(); itr != end(); ++itr) {
				itr->applyToAll(transformFunc);
			}
		}
		//Apply a function to every shape in the graphic
		void applyToAll(void(*transformFunc)(const shape&)) const {
			for (const_iterator itr = begin(); itr != end(); ++itr) {
				transformFunc(*itr);
			}
		}
		//Apply a function to every point in every shape in the graphic
		void applyToAll(void(*transformFunc)(const point&)) const {
			for (const_iterator itr = begin(); itr != end(); ++itr) {
				itr->applyToAll(transformFunc);
			}
		}
		//Set the origin for this graphic
		void setOrigin(point org) {
			for (iterator itr = begin(); itr != end(); itr++) {
				itr->setOrigin(org);
			}
		}
		//Rescale this graphic about the origin 
		void rescale(float scalingFactor) {
			//Iterate through this graphic's component shapes and rescale each one
			for (iterator itr = begin(); itr != end(); itr++) {
				itr->rescale(scalingFactor);
			}
		}
		//Rescale this graphic only in the x-dimension about the origin
		void rescaleX(float scalingFactor) {
			//Iterate through this graphic's component shapes and rescale each one
			for (iterator itr = begin(); itr != end(); itr++) {
				itr->rescaleX(scalingFactor);
			}
		}
		//Rescale this graphic only in the y-dimension about the origin
		void rescaleY(float scalingFactor) {
			//Iterate through this graphic's component shapes and rescale each one
			for (iterator itr = begin(); itr != end(); itr++) {
				itr->rescaleY(scalingFactor);
			}
		}
		//Get this graphic's bounding box
		const segment bounds() {
			//Declare the segment to be returned
			segment rets = front().bounds();
			//Use each shape's bounding box to find this shape's bounding box!
			for (iterator itr = begin(); itr != end(); itr++) {
				//This way we don't have to keep calling 'bounds'
				const segment checker = itr->bounds();
				//Check rightbounds
				if (checker.p2.x() > rets.p2.x()) {
					rets.p2.x(checker.p2.x());
				}
				//Check topbounds
				if (checker.p2.y() > rets.p2.y()) {
					rets.p2.y(checker.p2.y());
				}
				//Check leftbounds
				if (checker.p1.x() < rets.p1.x()) {
					rets.p1.x(checker.p1.x());
				}
				//Check bottombounds
				if (checker.p1.y() < rets.p1.y()) {
					rets.p1.y(checker.p1.y());
				}
			}
			//Return the augmented segment
			return rets;
		}
		//Squeezes this graphic between the lines y = 0 and y = 1, making it the standard width for one of these graphics
		void standardize() {
			//Make this graphic flush with the x and y axes
			setOrigin(bounds().p1);
			//Rescale this graphic to be flush with y = 1
			rescale(1 / (bounds().p2.x()));
		}
		//Fit this object to a new bounding box, breaking aspect ratio as neccessary
		void fitStretch(segment newBounds) {
			//These calculatoins all work best from the origin point
			standardize();
			//Rescale horizontally
			rescaleX(newBounds.p2.x());
			//Rescale vertically
			rescaleY(newBounds.p2.y());
			setOrigin(newBounds.p1 * -1.0f);
		}
		//Fit this object within a new bounding box without breaking the aspect ratio
		void fitWithin(segment newBounds) {
			//These calculatoins all work best from the origin point
			standardize();
			//Rescale to fit with x
			rescale(newBounds.p2.x() - newBounds.p1.x());
			//If it's too tall,
			if (bounds().height() > newBounds.height()) {
				//Rescale rationally to correct height
				rescale(newBounds.height() / bounds().height());
			}
			//Move the object to where it goes
			//First, let the object's origin be the center of its bounding box
			setOrigin(bounds().midpoint());
			//Finally, move the graphic to where it belongs
			setOrigin(newBounds.midpoint() * -1.0f);
		}
		//Fit this object to fill a new bounding box without breaking the aspect ratio
		void fitWithout(segment newBounds) {
			//These calculatoins all work best from the origin point
			standardize();
			//Rescale to fit with x
			rescale(newBounds.p2.x() - newBounds.p1.x());
			//If it isn't tall enough,
			if (bounds().height() < newBounds.height()) {
				//Rescale rationally to correct height
				rescale(newBounds.height() / bounds().height());
			}
			//Move the object to where it goes
			//First, let the object's origin be the center of its bounding box
			setOrigin(bounds().midpoint());
			//Finally, move the graphic to where it belongs
			setOrigin(newBounds.midpoint() * -1.0f);
		}
		//Default constructor
		graphic() : graphicContainer() {
			push_back(shape());
		}
		//Construct from a graphicContrainer
		graphic(const graphicContainer& shapedata) : graphicContainer(shapedata) {

		}
		//Copy constructor
		graphic(const graphic& other) : graphicContainer(other) {

		}
	};

	// Like a graphic, but meant to be used in animations
	class frame : public graphic {
	public:
		using graphic::iterator;
		using graphic::push_back;
		using graphic::insert;
		using graphic::erase;
		using graphic::size;
		using graphic::begin;
		using graphic::end;
		// How long should this frame stick around past one frame?
		int delay;
		//Default constructor (delay of 0.5 seconds)
		frame() : graphic() {
			delay = 0;
		}
		//Construct from delay and graphic
		frame(int del, graphic body) : graphic(body) {
			delay = del;
		}
	};

	/*Structures multiple graphics into an animated sprite*/
	class animation : public animationContainer {
	public:
		using animationContainer::iterator;
		using animationContainer::push_back;
		using animationContainer::erase;
		using animationContainer::insert;
		using animationContainer::size;
		using animationContainer::begin;
		using animationContainer::end;
		// REPRESENATION
		iterator currentframe; //Dynamic, meant for cycling through frames
		int frameclock; //Dynamic, meant for counting how long it's been on this frame.
		bool cycle; //Should this animation restart after it ends?
		// Default constructor
		animation() : animationContainer() {
			push_back(frame());
			currentframe = begin();
			frameclock = 0;
			cycle = true;
		}
		// Copy constructor
		animation(const animation& other) : animationContainer(other) {
			//Catch to the other animation's framewise iterator
			currentframe = begin();
			for (animation::const_iterator itr = other.begin(); itr != other.currentframe; ++itr, ++currentframe);
			frameclock = other.frameclock;
			cycle = other.cycle;
		}
		// Construct from an animation container type
		animation(bool cycle_, animationContainer framedata) : animationContainer(framedata) {
			currentframe = begin();
			frameclock = 0;
			cycle = cycle_;
		}
		// Return the graphic at the current frame, and advance the cloc
		const graphic& feed() {
			const graphic& ret = *currentframe;
			tick();
			return ret;
		}
		// Advance the frameclock by one, advancing if neccesary
		void tick() {
			if (frameclock < currentframe->delay) {
				frameclock++;
			}
			else {
				frameclock = 0;
				if (currentframe != end())
					currentframe++;
				else if (cycle) {
					currentframe = begin();
				}
			}
		}
		//Apply a function to every frame in the animation
		void applyToAll(void(*transformFunc)(frame&)) {
			for (iterator itr = begin(); itr != end(); ++itr) {
				transformFunc(*itr);
			}
		}
		//Apply a function to every shape in every frame in the animation
		void applyToAll(void(*transformFunc)(shape&)) {
			for (iterator itr = begin(); itr != end(); ++itr) {
				itr->applyToAll(transformFunc);
			}
		}
		//Apply a function to every point in every shape in every frame in the animation
		void applyToAll(void(*transformFunc)(point&)) {
			for (iterator itr = begin(); itr != end(); ++itr) {
				itr->applyToAll(transformFunc);
			}
		}
		//Apply a function to every frame in the animation
		void applyToAll(void(*transformFunc)(const frame&)) const {
			for (const_iterator itr = begin(); itr != end(); ++itr) {
				transformFunc(*itr);
			}
		}
		//Apply a function to every shape in every frame in the animation
		void applyToAll(void(*transformFunc)(const shape&)) const {
			for (const_iterator itr = begin(); itr != end(); ++itr) {
				itr->applyToAll(transformFunc);
			}
		}
		//Apply a function to every point in every shape in every frame in the animation
		void applyToAll(void(*transformFunc)(const point&)) const {
			for (const_iterator itr = begin(); itr != end(); ++itr) {
				itr->applyToAll(transformFunc);
			}
		}
	};


	//To be included as a member in animated objects
	class spritesheet {
	public:
		//Internal representation
		std::map<std::string, animation> animations;
		std::map<std::string, graphic> graphics;

	};

}

#include "fgrfileops.h"

#endif
