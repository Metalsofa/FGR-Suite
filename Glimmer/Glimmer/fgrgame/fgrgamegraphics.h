#pragma once
//The classes provided in this document have been optimized for runtime
//efficiency. In game development, it's better to use these.
#ifndef __FGR_GAME_DEV_GRAPHICS_H__
#define __FGR_GAME_DEV_GRAPHICS_H__

#include "fgrutils.h"

namespace fgr {

	namespace game {

		//This class supports animation and is very memory-efficient, as well as time efficient
		class sprite {
			std::pair<GLuint, GLsizei> GLhandle;
			bool defined;
		public:
			// REPRESENTATION
			//True if delays are in milliseconds, false if in frames.
			bool timeclock;
			//The delay between frames, either in milliseconds or frames
			unsigned int delay;
			//The frame we're currently on
			unsigned short currentframe;
			//Whether the animation should cycle or not
			bool cycle;
			//Whether the animation should even play
			bool play;

			//Assignment operator
			const sprite& operator=(const sprite& other) {
				GLhandle = other.GLhandle;
				defined = other.defined;
				timeclock = other.timeclock;
				delay = other.delay;
				currentframe = other.currentframe;
				cycle = other.cycle;
				play = other.play;
			}

			// CONSTRUCTORS
			//Default constructor
			sprite() {
				timeclock = true;
				currentframe = 0;
				cycle = false;
				play = true;
				GLhandle.first = 0;
				GLhandle.second = 0;
				delay = 100;
				defined = false;
			}

			//Construct from an animation object
			sprite(const animation& obj, bool timeclock_ = true, bool play_ = true) {
				timeclock = timeclock_;
				currentframe = 0;
				cycle = obj.cycle;
				play = play_;
				GLhandle = compile(obj);
				if (obj.size())
					delay = obj.front().delay;
				else
					delay = 100;
				defined = true;
			}

			//Construct from a graphic object
			sprite(const graphic& obj) {
				timeclock = false;
				currentframe = 0;
				cycle = false;
				play = false;
				delay = 100;
				GLhandle.first = compile(obj);
				GLhandle.second = 1;
				defined = true;
			}

			//Construct from a shape object
			sprite(const shape& obj) {
				timeclock = false;
				currentframe = 0;
				cycle = false;
				play = false;
				delay = 100;
				GLhandle.first = compile(obj);
				GLhandle.second = 1;
				defined = true;
			}

			//Construct from a glyph object
			sprite(const glyph& obj) {
				timeclock = false;
				currentframe = 0;
				cycle = false;
				play = false;
				delay = 100;
				GLhandle.first = compile(obj);
				GLhandle.second = 1;
				defined = true;
			}

			//Construct from a file path
			sprite(const std::string& path, bool timeclock_ = true, bool play_ = true) {
				//Figure the file extension
				switch (path[path.size() - 1]) {
				case 'n': {
					//Treat this as an animation
					fgr::animation obj;
					animationFromFile(obj, path);
					*this = sprite(obj, timeclock_, play_);
					return;
				}
				case 'r': {
					//Treat this as a graphic
					fgr::graphic obj;
					graphicFromFile(obj, path);
					*this = sprite(obj);
				}
				case 'h': {
					//Treat this as a shape
					fgr::shape obj;
					shapeFromFile(obj, path);
					*this = sprite(obj);
				}
				case 'l': {
					//Treat this as a glyph
					fgr::glyph obj;
					glyphFromFile(obj, path);
					*this = sprite(obj);
				}
				default:
					//This is an error, but we won't kill the program
					*this = sprite();
					return;
				}
			}

			// OTHER FUNCTIONS
			//Free up the GPU space that was taken up by this sprite
			void cleanup() {
				glDeleteLists(GLhandle.first, GLhandle.second);
				defined = false;
				return;
			}

			//DESTRUCTOR
			~sprite() {
				cleanup();
			}

		};

	}

}


#endif
