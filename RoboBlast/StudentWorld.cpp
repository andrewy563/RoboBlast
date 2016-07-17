#include "StudentWorld.h"
#include <string>
#include "Level.h" // required to use our provided class
#include "Actor.h"
using namespace std;

void StudentWorld::initializeStruct() {
	for (int i = 0; i < VIEW_HEIGHT; i++) {
		vector<Wall*> w;
		wallVec.push_back(w);
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
						Wall* w = new Wall(j, i);
						wallVec[i].push_back(w);
						break;
				}
			}
		}
	}
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
