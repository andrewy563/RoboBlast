#include "StudentWorld.h"
#include <string>
#include "Level.h" // required to use our provided class
#include "Actor.h"
using namespace std;

void StudentWorld::initializeStruct() {
	for (int i = 0; i < numSubLevel; i++) {
		vector<Actor*> w;
		actorVec.push_back(w);
	}
}


void StudentWorld::loadLevel()
{
	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel("level00.dat");
	if (result == Level::load_fail_file_not_found)
		cerr << "Could not find level00.dat data file\n";
	else if (result == Level::load_fail_bad_format)
		cerr << "Your level was improperly formatted\n";
	else if (result == Level::load_success)
	{
		for (int i = 0; i < VIEW_HEIGHT; i++) {
			for (int j = 0; j < VIEW_WIDTH; j++) {
				Level::MazeEntry ge = lev.getContentsOf(j, i, 0);
				switch (ge)
				{
				case Level::wall:
				{
					Wall* w = new Wall(j, i);
					actorVec[0].push_back(w);
					break;
				}
				case Level::player:
				{
					m_player = new Player(j, i, 0, this);
					break;
				}
				}
			}
		}
	}
}

bool StudentWorld::canIPass(int x, int y) {
	vector<Actor*> out;
	for (size_t i = 0; i < actorVec[0].size(); i++) {
		if (actorVec[0][i]->getX() == x && actorVec[0][i]->getY() == y && !actorVec[0][i]->passThrough()) {
			return false;
		}
	}
	return true;
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
