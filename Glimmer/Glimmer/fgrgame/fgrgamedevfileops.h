#pragma once

//Include this file if you wish to make use of the game-development file operations, such as loading game assets.

#ifndef __FGR_GAMEDEV_FILEOPS_H__
#define __FGR_GAMEDEV_FILEOPS_H__

#include "fgroverworld.h"

namespace fgr {

	namespace game {

		////////////////////////////////////////////////////////////////////////
		//		PROP BINARY STRUCTURE
		//
		//	| FLOAT X | FLOAT Y |
		//

		//Get a prop from a filestream, and advance the internal position indicator by that many characters.
		//prop fgetprop(FILE*& stream) {
		//	//This is the point that will be returned
		//	float buffer[2];
		//	fread(buffer, sizeof(float), 2, stream);
		//	return prop(buffer[0], buffer[1]);
		//}

		//Put a prop into a filestream, and advance the internal position indicator by that many bytes
		//void fputprop(const point& obj, FILE*& stream) {
		//	//This is the point that will be returned
		//	float buffer[2] = { obj.x(), obj.y() };
		//	fwrite(buffer, sizeof(float), 2, stream);
		//	return;
		//}


		///////////////////// SLIGHTLY MORE USER-FRIENDLY FILE READING/WRITING FUNCITONS //////////////////////////

		//Reads a prop out of the specified path and assigns it to the prop refrence passed in
		bool propFromFile(prop& object, const std::string& path) {
			FILE* fgpfile;
			fopen_s(&fgpfile, path.c_str(), "rb");
			if (!fgpfile) return false;
			//object = fgetprop(fgpfile);
			fclose(fgpfile);
			return true;
		}

		//Writes a prop object to a particular file path
		bool propToFile(const prop& object, const std::string& path) {
			FILE* fgpfile;
			fopen_s(&fgpfile, path.c_str(), "wb");
			if (!fgpfile) return false;
			//fputprop(prop, fgpfile);
			fclose(fgpfile);
			return true;
		}

		//Construct a prop from a file path using the above methods.
		prop::prop(const std::string& path) {
			propFromFile(*this, path);
		}

	}

}


#endif
