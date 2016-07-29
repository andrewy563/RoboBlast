#include "StudentWorld.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Level.h" // required to use our provided class
#include "Actor.h"
using namespace std;

StudentWorld::~StudentWorld() {
	cleanUp();
}

void StudentWorld::initializeStruct() {
	for (int i = 0; i <= getCurrentSubLevel(); i++) {
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
				case Level::fake_wall:
				{
					FakeWall* f = new FakeWall(j, i);
					actorVec[0].push_back(f);
					break;
				}
				case Level::gold:
				{
					Gold* g = new Gold(j, i, this);
					actorVec[0].push_back(g);
					break;
				}
				case Level::restore_health:
				{
					RestoreHealth* r = new RestoreHealth(j, i, this);
					actorVec[0].push_back(r);
					break;
				}
				case Level::ammo: 
				{
					Ammo* a = new Ammo(j, i, this);
					actorVec[0].push_back(a);
					break;
				}
				case Level::extra_life: {
					ExtraLife* e = new ExtraLife(j, i, this);
					actorVec[0].push_back(e);
					break;
				}
				}
			}
		}
	}
}

bool StudentWorld::canIPass(int x, int y) {
	for (size_t i = 0; i < actorVec[0].size(); i++) {
		if (actorVec[0][i]->getX() == x && actorVec[0][i]->getY() == y && !actorVec[0][i]->passThrough()) {
			return false;
		}
	}
	return true;
}

int StudentWorld::bulletColCheck(int x, int y) {
	for (size_t i = 0; i < actorVec[0].size(); i++) {
		if (actorVec[0][i]->getX() == x && actorVec[0][i]->getY() == y) {
			int col = actorVec[0][i]->bulletCollision();
			if (col == 1) {
				return 1;
			}
			if (col == 0) {
				return 0;
			}
		}
	}
	return 2;
}

void StudentWorld::placeObject(Actor* p) {
	actorVec[0].push_back(p);
}
void StudentWorld::cleanDead() {
	for (size_t i = 0; i < actorVec.size(); i++) {
		for (size_t j = 0; j < actorVec[i].size(); j++) {
			if (!actorVec[i][j]->alive()) {
				Actor* p;
				p = actorVec[i][j];
				vector<Actor*>::iterator it;
				it = actorVec[i].begin() + j;
				actorVec[i].erase(it);
				delete p;
			}
		}
	}
}

void StudentWorld::setDisplayText() {
	int score = getScore();
	int level = getLevel(); // (e.g. 03)
	int sublevel = getCurrentSubLevel(); // (e.g. 01. 00 is for main level)
	unsigned int timeLimit = m_time;
	int lives = getLives();
	// Next, create a string from your statistics, of the form:
	// Score: 0000100 Level: 03-01 Lives: 3 Health: 70% Ammo: 216 TimeLimit: 34
	string s = statTextFormatter(score, level, sublevel, lives, m_player->health() * 10 / 2, m_player->ammo(), timeLimit);
	// Finally, update the display text at the top of the screen with your
	// newly created stats
	setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

string StudentWorld::statTextFormatter(int score, int level, int sublevel, int lives, int health, int ammo, int time) {
	ostringstream oss;
	oss << "Score: ";
	oss.fill('0');
	oss << setw(7) << score;
	oss << "  Level: ";
	oss << setw(2) << level;
	oss << "-";
	oss << sublevel;
	oss << "  Lives: ";
	oss.fill(' ');
	oss << setw(2) << lives;
	oss << "  Health: ";
	oss << setw(3) << health << "%";
	oss << "  Ammo: ";
	oss << ammo;
	oss << "  Time: ";
	oss << setw(4) << time;
	string out = oss.str();
	return out;
}

Player* StudentWorld::player() {
	return m_player;
}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
