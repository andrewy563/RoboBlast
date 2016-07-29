#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <iostream>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
		m_numSubLevel = 0;
	
	}
	virtual ~StudentWorld();

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
		m_time = 3000;
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		if (m_time <= 0) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		setDisplayText();
		for (size_t i = 0; i < actorVec.size(); i++) {
			for (size_t j = 0; j < actorVec[i].size(); j++) {
				actorVec[i][j]->doSomething();
			}
		}
		m_player->doSomething();
		if (!m_player->alive()) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		m_time -= 1;
		cleanDead();
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		std::cerr << "hello";
		for (size_t i = 0; i < actorVec.size(); i++) {
			for (size_t j = 0; j < actorVec[i].size(); j++) {
				delete actorVec[i][j];
			}
			actorVec[i].clear();
		}
		actorVec.clear();
		delete m_player;
	}
    
    virtual int getCurrentSubLevel()
    {
        return m_numSubLevel; // This code is here merely to allow the skeleton to build.
    }

	bool canIPass(int x, int y);			// returns all actors that are at (x, y) in a certain sublevel
	int bulletColCheck(int x, int y);			// checks for a moveable object
	void placeObject(Actor* p);			// creates a new object at a certain point
	void initializeStruct();		// initializes the data structures
	void loadLevel();				// loads a level;
	void cleanDead();
	void setDisplayText();
	Player* player();
	std::string statTextFormatter(int score, int level, int sublevel, int lives, int health, int ammo, int time);
private:
	std::vector<std::vector<Actor*>> actorVec;
	Player* m_player;
	int m_numSubLevel;
	int m_time;

};

#endif // STUDENTWORLD_H_
