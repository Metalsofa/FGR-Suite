/*The purpose of this header file is to implement methods associated with
user-friendly fgr manipulation*/

#pragma once
#ifndef __graphics_h__
#define __graphics_h__

#include "geometry.h"
#include "crypt.h"

#include <sstream>
#include <string>
#include <vector>
#include <map>



//Here we define the class metastat, which holds three values, in RGB order.

class metastat {
public:
	int som;
	int emo;
	int cog;
	metastat(int somaticValue, int emotionalValue, int cognitiveValue) {
		som = somaticValue;
		emo = emotionalValue;
		cog = cognitiveValue;
	}

	metastat() {
		som = 0;
		emo = 0;
		cog = 0;
	}

	bool operator==(const metastat& other) const {
		return (other.som == som && other.emo == emo && other.cog == cog);
	}

	bool operator!=(const metastat& other) const {
		return !(other == *this);
	}

	void define(int newSom, int newEmo, int newCog) {
		som = newSom;
		emo = newEmo;
		cog = newCog;
	}

	//Returns the sum of all stat components
	int sum() const{ return (som + emo + cog); }

	//Returns the mean of all stat components
	int mean() const{ return (sum() / 3); }

	//1: Somatic 2: Emotional 3: Cognitive 4: Mean else: sum
	int component(int comp) {
		if (comp == 1)
			return som;
		if (comp == 2)
			return emo;
		if (comp == 3)
			return cog;
		if (comp == 4)
			return mean();
		else 
			return sum();
	}
};


//255 - each stat's value for 
inline metastat inverse(metastat &base) {
	return metastat(255-base.som, 255-base.emo, 255-base.cog);
}


class shape {
public:
	vector<point> vertices;
	metastat color;
	float opacity;
	int mode;
	float lineThickness;

	//Returns the shape's encrypted string
	string cryptogram() {
		string translation;
		translation = to_string(vertices.size()); ///The first argument is an integer counting the number of verticies
		for (int i = 0; i < vertices.size(); i++) {
			translation += " "
				+ to_string(vertices[i].x())
				+ " "
				+ to_string(vertices[i].y());
		}
		return translation + " "
			+ to_string(color.som) + " "
			+ to_string(color.emo) + " "
			+ to_string(color.cog) + " "
			+ to_string(opacity) + " "
			+ to_string(mode) + " "
			+ to_string(lineThickness) + " ";
	}

	//Write this shape's contents to a text file
	void savetofile(string& filename) { //DP: Seems to be repetitive with cryptogram, just make 1 function.
		vector<string> translation(2, "");
		translation[0] = to_string(vertices.size()); ///The first argument is an integer counting the number of verticies
		for (int i = 0; i < vertices.size(); i++) {
			translation[0] += " ";
			translation[0] += to_string(vertices[i].x());
			translation[0] += " ";
			translation[0] += to_string(vertices[i].y());
		}
		translation[0] += " ";
		translation[0] += to_string(color.som) + " ";
		translation[0] += to_string(color.emo) + " ";
		translation[0] += to_string(color.cog) + " ";
		translation[0] += to_string(opacity) + " ";
		translation[0] += to_string(mode) + " ";
		translation[0] += to_string(lineThickness) + " ";
		encryptAndOverwrite(translation, filename, "The Doors of Perception");
	}

	//Default constructor
	shape() {
		color = metastat(60, 60, 60);
		opacity = 1.0f;
		mode = 5;
		lineThickness = 1.0f;
	}

	//Initialize a shape by reading it from a string
	shape(string& text) {
		///Container 0: vertices
		stringstream reader(text);
		int vertexcount;
		reader >> vertexcount;
		for (int j = 0; j < vertexcount; j++) {
			float X; reader >> X;
			float Y; reader >> Y;
			point vert(X, Y);
			vertices.push_back(vert);
		}
		///Extract color
		reader >> color.som; reader >> color.emo; reader >> color.cog;
		///Extract opacity
		reader >> opacity;
		///Extract mode
		reader >> mode;
		///Extract lineThickness
		reader >> lineThickness;
	}

	//Initialize a shape by reading it from a file, telling it where to begin reading
	shape(string filename, int begin) { //DP: repetitive with func above
		vector<string> shapeContents = unencryptedContents(filename, "The Doors of Perception");
		if (shapeContents.size() >= 1) {
			///Container 0: vertices
			stringstream reader(shapeContents[begin]);
			int vertexcount;
			reader >> vertexcount;
			for (int j = 0; j < vertexcount; j++) {
				float X; reader >> X;
				float Y; reader >> Y;
				point vert(X, Y);
				vertices.push_back(vert);
			}
			///Extract color
			reader >> color.som; reader >> color.emo; reader >> color.cog;
			///Extract opacity
			reader >> opacity;
			///Extract mode
			reader >> mode;
			///Extract lineThickness
			reader >> lineThickness;
		}
	}
	//Translate every vertex relative to the origin
	void setOrigin(point org) {
		for (int i = 0; i < vertices.size(); i++) {
			vertices[i] = difference(vertices[i], org);
		}
	}
	//Scale this shape around the origin
	void rescale(float scalingFactor) {
		for (unsigned long i = 0; i < vertices.size(); i++) {
			vertices[i] *= scalingFactor;
		}
	}
	//Scale this shape strictly in the x-dimension around the origin
	void rescaleX(float scalingFactor) {
		for (unsigned long i = 0; i < vertices.size(); i++) {
			vertices[i].xmult(scalingFactor);
		}
	}
	//Scale this shape strictly in the y-dimension around the origin
	void rescaleY(float scalingFactor) {
		for (unsigned long i = 0; i < vertices.size(); i++) {
			vertices[i].ymult(scalingFactor);
		}
	}
	//Returns the diagonally-spanning segment for this shape
	const segment bounds() const {
		segment rets = segment(vertices[0], vertices[1]);
		//Iterate through every point and set the bounding segment accordingly
		for (unsigned int i = 0; i < vertices.size(); i++) {
			//Check rightbounds
			if (vertices[i].x() > rets.p2.x()) {
				rets.p2.x(vertices[i].x());
			}
			//Check topbounds
			if (vertices[i].y() > rets.p2.y()) {
				rets.p2.y(vertices[i].y());
			}
			//Check leftbounds
			if (vertices[i].x() < rets.p1.x()) {
				rets.p1.x(vertices[i].x());
			}
			//Check bottombounds
			if (vertices[i].y() < rets.p1.y()) {
				rets.p1.y(vertices[i].y());
			}
		}
		//Now rets is the bounding segment for this shape!
		return rets;
	}
	//Returns the name of the GL drawing mode associated with this shape
	string getGLMODE() {
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
		cerr << "Something happened in players.h/shape/getGLMODE that isn't supposed to..." << endl;
		return "ERR_NOT_GLMODE";
	}
};

class graphic {
public:
	vector<shape> pieces; //This alone gives the graphic form.
	int delay = 0; //Optional member, only useful if this graphic is in an animation.

	//Set the origin for this graphic
	void setOrigin(point org) {
		for (int i = 0; i < pieces.size(); i++) {
			pieces[i].setOrigin(org);
		}
	}

	//Rescale this graphic about the origin 
	void rescale(float scalingFactor) {
		//Iterate through this graphic's component shapes and rescale each one
		for (unsigned long i = 0; i < pieces.size(); i++) {
			pieces[i].rescale(scalingFactor);
		}
	}

	//Rescale this graphic only in the x-dimension about the origin
	void rescaleX(float scalingFactor) {
		//Iterate through this graphic's component shapes and rescale each one
		for (unsigned long i = 0; i < pieces.size(); i++) {
			pieces[i].rescaleX(scalingFactor);
		}
	}

	//Rescale this graphic only in the y-dimension about the origin
	void rescaleY(float scalingFactor) {
		//Iterate through this graphic's component shapes and rescale each one
		for (unsigned long i = 0; i < pieces.size(); i++) {
			pieces[i].rescaleY(scalingFactor);
		}
	}

	//Get this graphic's bounding box
	const segment bounds() {
		//Declare the segment to be returned
		segment rets = pieces[0].bounds();
		//Use each shape's bounding box to find this shape's bounding box!
		for (unsigned int i = 0; i < pieces.size(); i++) {
			//This way we don't have to keep calling 'bounds'
			const segment checker = pieces[i].bounds();
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
	graphic() {
		pieces.push_back(shape());
	}

	//Initialize this graphic from a file
	graphic(string filename) {
		vector<string> contents = unencryptedContents(filename, "The Doors of Perception");
		for (string& line : contents) {
			pieces.push_back(shape(line));
		}
	}

	//Save this graphic to a file
	void savetofile(string filename) {
		vector<string> lines;
		for (shape& piece : pieces) {
			lines.push_back(piece.cryptogram());
		}
		encryptAndOverwrite(lines, filename, "The Doors of Perception");
	}

	//Translate this graphic into a string
	string filestring() {
		vector<string> sublines;
		for (unsigned int i = 0; i < pieces.size(); i++) {
			sublines.push_back(pieces[i].cryptogram());
		}
		string rets = "{\n";
		for (unsigned int i = 0; i < sublines.size(); i++) {
			rets += "\t\t" + sublines[i] + ";\n";
		}
		rets.push_back('\t');
		rets.push_back('}');
		return rets;
	}
};

/*Structures multiple graphics into an animated sprite*/
class animation {
public:
	vector<graphic> frames;
	int framenumber; //Dynamic, meant for cycling through frames
	int frameclock; //Dynamic, meant for counting how long it's been on this frame.
	bool cycle; //Should this animation restart after it ends?

	animation() {
		framenumber = 0;
		frameclock = 0;
		cycle = true;
		frames.push_back(graphic());
	}

	graphic& operator[] (int frame) {
		return frames[frame];
	}

	void tick() {
		if (frameclock < frames[framenumber].delay) {
			frameclock++;
		}
		else {
			frameclock = 0;
			if (framenumber < frames.size() - 1)
				framenumber++;
			else if (cycle) {
				framenumber = 0;
			}
		}
	}
	//Translate this animation to an (unencrypted) string
	string filestring() {
		vector<string> graphica;
		for (unsigned int i = 0; i < frames.size(); i++) {
			graphica.push_back(frames[i].filestring());
		}
		string rets = "{\n";
		for (unsigned int i = 0; i < graphica.size(); i++) {
			rets += "\t" + graphica[i] + '\n';
		}
		rets.push_back('}');
	}

	//Translate an unencrypted string into an animation
	animation(string fstring) {

	}
};


//To be included as a member in animated objects
class spritesheet {
public:
	map<string, animation> animations;
	map<string, graphic> graphics;

	//Translate this entire spritesheet into a big (unencrypted) string
	string cryptogram() {
		/*Uh oh I have no Idea how to iterate through maps*/
		
	}
};

#endif