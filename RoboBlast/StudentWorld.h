#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <iostream>

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
		if (getLevel() >= 100) {				// if player has completed level 99, player automatically wins
			return GWSTATUS_PLAYER_WON;
		}

		initializeStruct();						// create all data structures required
		int out = fileNameHandler();				// process file names and load the level
		m_time = 3000;							// default time is 3000
		m_levelEnd = false;						// level has not ended yet
		OPEN_EXIT = false;						// exit should be closed
		m_numSubLevel = 0;
		return out;
	}

	virtual int move()
	{
		m_time -= 1;							// subtract the tick count by 1

		if (m_time <= 0) {						// if the time has reached 0, automatically kill the player
			decLives();
			m_numSubLevel = 0;
			return GWSTATUS_PLAYER_DIED;
		}

		setDisplayText();						// update the display text

		int originalSubLevel = getCurrentSubLevel();	// keep track of the original sublevel

		size_t size = actorVec[getCurrentSubLevel()].size();
		for (size_t j = 0; j < size; j++) {
			if (!actorVec[getCurrentSubLevel()][j]->getID() == IID_PLAYER) {		// player will call its doSomething() after the loop
				actorVec[getCurrentSubLevel()][j]->doSomething();
				if (originalSubLevel != getCurrentSubLevel()) {		// if sublevels have been moved, then break from the loop
					j = size;
				}
			}
		}

		m_player->doSomething();									// player's turn to do something

		equalizeStats();											// equalize stats among all player objects that are on different sublevels
	
		if (!m_player->alive()) {									// if player died, decrease lives by 1
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}

		openExit();													// check if exit should be opened

		if (m_levelEnd) {											// if player completed the level, end the level and increase the score
			increaseScore(m_time);
			return GWSTATUS_FINISHED_LEVEL;
		}

		  // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		  // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.

		cleanDead();												// remove all dead Actors
		return GWSTATUS_CONTINUE_GAME;								// game continues...
	}

	virtual void cleanUp()
	{
		// delete all pointers in actorVec and then clear actorVec
		for (size_t i = 0; i < actorVec.size(); i++) {
			for (size_t j = 0; j < actorVec[i].size(); j++) {
				delete actorVec[i][j];
			}
			actorVec[i].clear();
		}
		actorVec.clear();
	}
    
    virtual int getCurrentSubLevel()
    {
        return m_numSubLevel; 
    }

	bool canIPass(int x, int y, int sublevel);										// returns all actors that are at (x, y) in a certain sublevel
	int bulletColCheck(int x, int y, int sublevel);									// checks if bullet colides into an object at (x, y)
	void placeObject(Actor* p, int sublevel);										// adds an Actor object to actorVec
	void initializeStruct();														// initializes the data structures
	int loadLevel(std::string name, int sublevel);									// loads a level;
	void cleanDead();																// deletes all dead Actors
	void setDisplayText();															// updates display text
	int fileNameHandler();															// processes the file name of the level data file
	void setSubLevel(int sl);														// moves current sublevel to sublevel sl 
	void endLevel();																// ends the level
	void openExit();																// activates the exit
	void equalizeStats();															// equalizes stats among all Player objects in various sublevels
	int getTick();																	// returns the current number of ticks
	bool checkGrid(int dim, int ID, int sublevel, Actor* center, Actor*& output);	// checks the grid for a specific Actor
	int countGrid(int dim, int ID, int sublevel, Actor* center);					// counts how many Actors of type ID are around the center Actor
	Player* player();																// returns a pointer to the main player
	std::string statTextFormatter(int score, int level, int sublevel, int lives, int health, int ammo, int time);	// formats display text

private:
	std::vector<std::vector<Actor*>> actorVec;										// container of all the Actors
	Player* m_player;																// pointer to the main player
	int m_numSubLevel;																// keeps track of what sublevel StudentWorld is on
	int m_time;																		// how many ticks are left
	bool m_levelEnd;																// whether or not level has ended
	bool OPEN_EXIT;																	// whether or not exit is open
};

#endif // STUDENTWORLD_H_