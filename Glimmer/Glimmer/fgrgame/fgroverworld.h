/*Overworld type: room. In order for the overworld to be rendered at all,
the player has to be in some sort of room.*/
#pragma once

#ifndef __FGR_room_h__
#define __FGR_room_h__

#include "fgrutils.h"
#include <map>

namespace fgr {

	namespace game {

		//Forward declaration
		class world;
		class prop;
		class door;
		class room;

		//The maximum number of rooms that can be loaded in at once.
		const unsigned int max_rooms = 16;

		///////////////////////////////////////////////////////////
		//
		//           Class room DEFINITION
		//
		//				This class structures the properties
		//				of a given room.
		//				Used as a node in the 'world' data
		//				structure.
		//
		///////////////////////////////////////////////////////////

		//A single room in a world, loaded in all at once
		class room {
		public:
			// REPRESENTATION
			//If this is true, something could be loaded on top of this room
			bool empty;
			//Hook to the world class that this room is a part of 
			world* worldhook;
			//Identical to the filepath this was loaded from
			std::string name;
			//The image representing the background
			fgr::game::multisprite background;
			//The portals from this room to other rooms, and which room they go to
			std::vector<door> gateways;
			//How far from the player's location we have found this room to be
			unsigned short traversalDistance;

			// FUNCTIONS
			//CONSTRUCTORS
			//Default constructor
			room();
			//Construct from a filepath
			room(const std::string& path);
			//OTHER FUNCTIONS
			//Load neighbors of this room
			void loadneighbors(int depth, int ignore);
			//Free the slot this room takes and disconnect it from any rooms that connect to it
			void freeme();
			//Recover a freed room
			void recover();
		};

		///////////////////////////////////////////////////////////
		//
		//           Class world DEFINITION
		//
		//			This class structures the rooms
		//			that are loaded in the game.
		//			This implementation uses a graph
		//			structure that keeps the members
		//			in a vector. This makes garbage
		//			collection a lot easier.
		//
		///////////////////////////////////////////////////////////

		//A data structure for organizing rooms in a linked graph
		class world {
		public:
			// REPERSENTATION
			//The rooms are kept in this array
			room rooms[max_rooms];
			//The 'current' room
			room* currentRoom;
			//How far from a room we are willing to be before despawning it automatically.
			unsigned short maxdistance;
			// FUNCTIONS
			//Traverse he world and set the stepcount for the rooms
				//Driver funciton
			void setDistances();
				//For recursive use
			void setDistances(room* travis, unsigned short dist);
			//Load a room file into the room array automatically
			room* loadroom(const std::string& filepath);
			//Finds space in the room array for a new room to be put in there
			unsigned int findspace();
			// FUNCTIONS
			//CONSTRUCTORS
			//Default constructor
			world();
			//Iterates through the room array and frees cells that are no longer needed
			void cleanup();
		};

		///////////////////////////////////////////////////////////
		//
		//           Class prop DEFINITION
		//
		//				These are objects that can be interacted
		//				with (or not) in the world
		//
		///////////////////////////////////////////////////////////

		//When a prop is loaded, it knows what function to associate with using this.
		std::map < std::string, void(*)(prop& object)> prop_functions;

		//These give life to the world
		class prop : public fgr::spritesheet {	//DP: I think the prop should have a spritesheet, rather than be a spritesheet
		public:
			// REPRESENTATION
			//Structures the graphics of this prop
			fgr::spritesheet appearance;
			//Identical to the filepath this was loaded from
			std::string name;
			//Position relative to the room's origin point
			fgr::point position;
			//Whether or not this prop can be moved.
			bool movable;
			//How many times this prop remembers being interacted with by the player previously
			int interactionCount;
			//Local 'variables' for this prop
			std::map<std::string, std::string> variables;
			//This function will be called when the player interacts with this prop
			void(*interact_func)(prop& object);	//DP: Does a prop need to be passed in?

			// MEMBER FUNCTIONS
			//CONSTURCTORS
			//Default constructor
			prop();
			//Construct from a file path
			prop(const std::string& path);
			//Call this when the player interacts with the prop (confusing I know)
			void interact();
		};
		
		//The following function should be used during program initialization
		//To map an interaction function to any prop loaded with that name
		void setPropFunc(const std::string& propname, void(*interact_func)(prop& object)) {
			prop_functions[propname] = interact_func;
			return;
		}


		///////////////////////////////////////////////////////////
		//
		//           Class door DEFINITION
		//
		//				These act as pointers in the
		//				'world' data structure.
		//
		///////////////////////////////////////////////////////////

		class door {
		public:
			//Pointer to the room this door is a part of 
			room* parent;
			//The box the player must walk into in order to pass through the door
			fgr::segment hitbox;
			//The file path to the room that this door leads to
			std::string filepath;
			//The identity of the room within this floor that this leads to
			room* destination;
			//The index of the door in the destination room that this one sends your out of
			unsigned int destdoor;
			//Whether the room it points to should be loaded as soon as this one is (part of the same contiguous reigon, in other words)
			bool contiguous;
			//Whether the player may go through this door, otherwise it is 'locked' and causes no interaction
			bool maypass;
			// FUNCTIONS
			//CONSTRUCTORS
			//Default constructor
			door();
			//The derefrence operator returns a refrence to the room this door points to
			room& operator*() const;
		};

	}

}


namespace fgr {

	namespace game {

		// WORLD CLASS IMPLEMENTATION

		//Default constructor
		world::world() {
			for (unsigned int i = 0; i < max_rooms; ++i) {
				rooms[i].empty = true;
				rooms[i].worldhook = this;
			}
			return;
		}

		//Free up any room slots that are no longer needed
		void world::cleanup() {
			setDistances();
			for (unsigned int i = 0; i < max_rooms; ++i) {
				if (!rooms[i].empty && rooms[i].traversalDistance > maxdistance) {
					rooms[i].freeme();
				}
			}
			return;
		}

		//Recursive component of traversing the world and setting distances
		void world::setDistances(room* travis, unsigned short dist) {
			assert(travis->traversalDistance == 0);
			travis->traversalDistance = dist;
			for (unsigned int i = 0; i < travis->gateways.size(); ++i) {
				if (travis->traversalDistance > dist) {
					setDistances(travis, dist + 1);
				}

			}
			return;
		}

		//Traverse the world and mark each room with how many steps it took to get there
		void world::setDistances() {
			//Reset the distances to 0
			for (unsigned int i = 0; i < max_rooms; ++i) {
				rooms[i].traversalDistance = 0;
			}
			//Traverse the world
			setDistances(currentRoom, 1);
			//After this proccess, there should be no zeroes. They would be unreachable.
			return;
		}

		//Find space for a new room in the room array
		unsigned int world::findspace() {
			int furthest = 0;
			for (unsigned int i = 0; i < max_rooms; ++i) {
				//If there's a free space, just return that
				if (rooms[i].empty) return i;
				if (rooms[i].traversalDistance > rooms[furthest].traversalDistance)
					furthest = i;
			}
			//Otherwise we need to make room (assume distances are already set)
			rooms[furthest].freeme();
			return furthest;
		}

		//Loads a room file into the world and returns a pointer to it
		room* world::loadroom(const std::string& filepath) {
			//Search for if the room is already present
			for (unsigned int i = 0; i < max_rooms; ++i) {
				if (rooms[i].name == filepath) {
					rooms[i].recover();
					return &(rooms[i]);
				}
			}
			//Otherwise find room for it
			room* slot = rooms + findspace();
			*slot = room(filepath);
			return slot;
		}

		// PROP CLASS IMPLEMENTATION
		prop::prop() {
			movable = false;
			interact_func = NULL;
			interactionCount = 0;
		}

		//Call this to trigger interaction with this prop
		void prop::interact() {
			interact_func(*this);
			interactionCount++;
		}

		// DOOR CLASS IMPLEMENTATION

		//Default constructor
		door::door() {
			destination = NULL;
			contiguous = false;
			maypass = true;
		}

		//Dereference operator
		room& door::operator*() const {
			return *destination;
		}

		// ROOM CLASS IMPLEMENTATION

		//Default constructor
		room::room() {
			empty = true;
			worldhook = NULL;
		}

		//Construct from room file path
		room::room(const std::string& filepath) {

		}

		//Disconnect this room from any contiguous ones, and then free it up.
		void room::freeme() {
			//Unlink any rooms connected to this one.
			for (unsigned int i = 0; i < gateways.size(); ++i) {
				gateways[i].destination->gateways[gateways[i].destdoor].destination = NULL;
				gateways[i].destination = NULL;
			}
			empty = true;
		}

		//Get back a room that has been freed.
		void room::recover() {
			loadneighbors(1, -1);
		}

		//Load the neighbors of this room, recursively possibly loading the neighbors as well
		void room::loadneighbors(int depth, int ignore) {
			//Iterate through every door
			door* mydoor;
			door* theirdoor;
			for (unsigned int i = 0; i < gateways.size(); ++i) {
				//Make sure to ignore the one we want to ignore
				if (i == ignore) continue;
				mydoor = &gateways[i];
				if (depth > 0 || mydoor->contiguous) {
					if (!mydoor->destination) {
						mydoor->destination = worldhook->loadroom(mydoor->filepath);
						theirdoor = &mydoor->destination->gateways[mydoor->destdoor];
						theirdoor -> destination = this;
						theirdoor -> destdoor = i;
					}
					(**mydoor).loadneighbors(depth - 1, mydoor->destdoor);
				}
			}
			return;
		}
	}
}

#endif