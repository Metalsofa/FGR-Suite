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
			friend class motionsprite;
			// REPRESENTATION
			//The delay between frames, in frames.
			unsigned int delay;
			//The frame we're currently on
			unsigned int currentframe;
			//How many frames we've been on this frame
			unsigned int frameclock;
			//Whether the animation should cycle or not
			bool cycle;
			//Whether the animation should even play
			bool play;

			//Restart this animaiton at the beginning
			void restart() {
				frameclock = 0;
				currentframe = 0;
			}

			void resume() { play = true; }

			void pause() { play = false; }

			//Restart and replau
			void replay() {
				restart();
				resume();
			}

			//Call the list, advance to the next frame
			void draw() {
				glCallList(GLhandle.first + currentframe);
				if (play) {
					++frameclock;
					if (frameclock > delay) {
						frameclock = 0;
						++currentframe;
						if (currentframe > GLhandle.second) {
							if (cycle) {
								currentframe = 0;
							}
							else {
								--currentframe;
								pause();
							}
						}
					}
				}
			}

			//Assignment operator
			const sprite& operator=(const sprite& other) {
				GLhandle = other.GLhandle;
				defined = other.defined;
				delay = other.delay;
				currentframe = other.currentframe;
				cycle = other.cycle;
				play = other.play;
				frameclock = other.frameclock;
			}

			// CONSTRUCTORS
			//Default constructor
			sprite() {
				currentframe = 0;
				cycle = true;
				play = true;
				GLhandle.first = 0;
				GLhandle.second = 0;
				delay = 0;
				defined = false;
				frameclock = 0;
			}

			//Construct from an animation object
			sprite(const animation& obj, bool timeclock_ = true, bool play_ = true) {
				currentframe = 0;
				frameclock = 0;
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
				currentframe = 0;
				frameclock = 0;
				cycle = false;
				play = false;
				delay = 100;
				GLhandle.first = compile(obj);
				GLhandle.second = 1;
				defined = true;
			}

			//Construct from a shape object
			sprite(const shape& obj) {
				currentframe = 0;
				frameclock = 0;
				cycle = false;
				play = false;
				delay = 100;
				GLhandle.first = compile(obj);
				GLhandle.second = 1;
				defined = true;
			}

			//Construct from a glyph object
			sprite(const glyph& obj) {
				currentframe = 0;
				frameclock = 0;
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
				if (defined) {
					glDeleteLists(GLhandle.first, GLhandle.second);
					defined = false;
				}
				return;
			}

			//DESTRUCTOR
			~sprite() {
				cleanup();
			}

		};

		//A set of instructions that may have different lengths and delay times.
		class motionsprite {
		public:
			sprite artframes;
			sprite xpososcillationframes;
			sprite ypososcillationframes;
			sprite rotoscillationframes;
			sprite scaleoscillationframes;
			//sprite pathframes; ///Still needs to be implemented
			//Draw this sprite
			void draw() {
				glPushMatrix();
					//pathframes.draw();
					xpososcillationframes.draw();
					ypososcillationframes.draw();
					rotoscillationframes.draw();
					scaleoscillationframes.draw();
					artframes.draw();
				glPopMatrix();
			}
			//Constructors
			motionsprite(const component& art, float framerate = 60.0f) {
				artframes = sprite(art);
				//The size of the oscillation array, in frames
				GLsizei resolution;

				//X-positional oscillation
				if (art.posfreq.x() > 0.001) {
					resolution = framerate / art.posfreq.x();
					xpososcillationframes = sprite();
					xpososcillationframes.GLhandle.first = glGenLists(resolution);
					xpososcillationframes.GLhandle.second = resolution;
					for (GLuint i = 0; i < resolution; ++i) {
						glNewList(xpososcillationframes.GLhandle.first + i, GL_COMPILE);
							glTranslatef(art.position.x() + art.posamp.x() * cosf(float(i) / float(resolution) * (PI * 2)), 0.0f, 0.0f);
						glEndList();
					}
					xpososcillationframes.defined = true;
				}

				//Y-positional oscillation
				if (art.posfreq.y() > 0.001)
					resolution = framerate / art.posfreq.y();
				else
					resolution = 0;
				ypososcillationframes = sprite();
				ypososcillationframes.GLhandle.first = glGenLists(resolution);
				ypososcillationframes.GLhandle.second = resolution;
				for (GLuint i = 0; i < resolution; ++i) {
					glNewList(ypososcillationframes.GLhandle.first + i, GL_COMPILE);
						glTranslatef(0.0f, art.position.y() + art.posamp.y * (float(i) / float(resolution) * (PI * 2)), 0.0f);
					glEndList();
				}
				ypososcillationframes.defined = true;

				//Rotational oscillation
				if (art.rotfreq > 0.001) {
					resolution = framerate / art.rotfreq;
					rotoscillationframes = sprite();
					rotoscillationframes.GLhandle.first = glGenLists(resolution);
					rotoscillationframes.GLhandle.second = resolution;
					for (GLuint i = 0; i < resolution; ++i) {
						glNewList(rotoscillationframes.GLhandle.first + i, GL_COMPILE);
							glRotatef(art.rotation + float(i) / float(resolution) * 360.0f, 0.0f, 0.0f, 1.0f);
						glEndList();
					}
					rotoscillationframes.defined = true;
				}

				//Scaling oscillation
				if (art.scalefreq > 0.001) {
					resolution = framerate / art.scalefreq;
					scaleoscillationframes = sprite();
					scaleoscillationframes.GLhandle.first = glGenLists(resolution);
					scaleoscillationframes.GLhandle.second = resolution;
					float magnitude;
					for (GLuint i = 0; i < resolution; ++i) {
						glNewList(scaleoscillationframes.GLhandle.first + i, GL_COMPILE);
							magnitude = art.scale + art.scaleamp * sinf(float(i) / float(resolution) * (PI * 2));
							glScalef(magnitude, magnitude, magnitude);
						glEndList();
					}
					scaleoscillationframes.defined = true;
				}

			}
		};

		//A compiled image class with many moving parts
		class multisprite {
			//These are the video data's many dimensions
			std::vector<motionsprite> video;
		public:
			// CONSTRUCTORS
			//Default constructor
			multisprite() {

			}

			//Construct from a painting
			multisprite(const fgr::painting& art, float framerate = 60.0f) {
				video.resize(art.size());
				int i = 0;
				for (painting::const_iterator itr = art.begin(); itr != art.end(); ++itr, ++i) {
					video[i] = motionsprite(*itr, framerate);
				}
			}

		};

	}

}


#endif
