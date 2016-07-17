#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Wall;
class Player;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
	}

	virtual int init()
	{
		initializeStruct();
		loadLevel();
		/*1. Initialize the data structures used to keep track of your game’s world.
2. Load the current main maze’s and all its sub-mazes’ details from level data file(s).
3. Allocate and insert a valid Player object into the game world.
4. Allocate and insert any Gangster objects, Bully objects, Robot Boss objects, Wall
objects, Fake Wall objects, Waterpool objects, Hostage objects, Bully Nest
objects, Gold Bullion objects, Goodie objects, Farplane Gun objects, or Exit
objects into the game world, as required by the specification in the level’s data
file. */

		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		decLives();
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
	}
    
    virtual int getCurrentSubLevel()
    {
        return 0; // This code is here merely to allow the skeleton to build.
    }

	void initializeStruct();		// initializes the data structures
	void loadLevel();				// loads a level;
private:
	std::vector<std::vector<Wall*>> wallVec;
	Player* player;
};

#endif // STUDENTWORLD_H_
