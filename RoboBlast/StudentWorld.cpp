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
	for (int i = 0; i <= MAX_SUB_LEVEL; i++) {
		vector<Actor*> w;
		actorVec.push_back(w);
	}
}


int StudentWorld::loadLevel(string name, int sublevel)
{
	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(name, sublevel);
	if (result == Level::load_fail_file_not_found) {
		cerr << "Could not find " << name << " data file\n";
		return GWSTATUS_PLAYER_WON;
	}
	else if (result == Level::load_fail_bad_format || result == Level::load_sublevel_fail_bad_format) {
		cerr << "Your level was improperly formatted\n";
		return GWSTATUS_LEVEL_ERROR;
	}
	else if (result == Level::load_success || result == Level::load_sublevel_success)
	{
		cerr << "loading " << name << endl;
		for (int i = 0; i < VIEW_HEIGHT; i++) {
			for (int j = 0; j < VIEW_WIDTH; j++) {
				Level::MazeEntry ge = lev.getContentsOf(j, i, sublevel);
				switch (ge)
				{
				case Level::wall:
				{
					Wall* w = new Wall(j, i, sublevel);
					actorVec[sublevel].push_back(w);
					break;
				}
				case Level::player:
				{
					Player *p = new Player(j, i, this, sublevel);
					if (sublevel == 0) {
						m_player = p;
					}
					playerVec.push_back(p);
					break;
				}
				case Level::fake_wall:
				{
					FakeWall* f = new FakeWall(j, i, sublevel);
					actorVec[sublevel].push_back(f);
					break;
				}
				case Level::gold:
				{
					Gold* g = new Gold(j, i, this, sublevel);
					actorVec[sublevel].push_back(g);
					break;
				}
				case Level::restore_health:
				{
					RestoreHealth* r = new RestoreHealth(j, i, this, sublevel);
					actorVec[sublevel].push_back(r);
					break;
				}
				case Level::ammo:
				{
					Ammo* a = new Ammo(j, i, this, sublevel);
					actorVec[sublevel].push_back(a);
					break;
				}
				case Level::extra_life: {
					ExtraLife* e = new ExtraLife(j, i, this, sublevel);
					actorVec[sublevel].push_back(e);
					break;
				}
				case Level::hostage: {
					Hostage* h = new Hostage(j, i, this, sublevel);
					actorVec[sublevel].push_back(h);
					break;
				}
				case Level::horiz_gangster: {
					Gangster* g = new Gangster(j, i, 1, this, sublevel);
					actorVec[sublevel].push_back(g);
					break;
				}
				case Level::vert_gangster: {
					Gangster* g = new Gangster(j, i, 2, this, sublevel);
					actorVec[sublevel].push_back(g);
					break;
				}
				case Level::robot_boss: {
					Robot* r = new Robot(j, i, this, sublevel);
					actorVec[sublevel].push_back(r);
					break;
				}
				case Level::exit: {
					if (sublevel == 0) {
						Exit* e = new Exit(j, i, this, sublevel);
						actorVec[sublevel].push_back(e);
					}
					break;
				}
				case Level::farplane_gun: {
					Farplane* f = new Farplane(j, i, this, sublevel);
					actorVec[sublevel].push_back(f);
					break;
				}
				case Level::bully_nest: {
					Nest* n = new Nest(j, i, this, sublevel);
					actorVec[sublevel].push_back(n);
					break;
				}
				default: {
					if (ge >= 0 && ge <= 5) {
						Gate* g = new Gate(j, i, this, sublevel, ge);
						actorVec[sublevel].push_back(g);
					}
					break;
				}
				}
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::canIPass(int x, int y, int sublevel) {
	for (size_t i = 0; i < actorVec[sublevel].size(); i++) {
		if (actorVec[sublevel][i]->getX() == x && actorVec[sublevel][i]->getY() == y && !actorVec[sublevel][i]->passThrough()) {
			return false;
		}
	}
	return true;
}

int StudentWorld::bulletColCheck(int x, int y, int sublevel) {
	int out = 2;
	for (size_t i = 0; i < actorVec[sublevel].size(); i++) {
		if (actorVec[sublevel][i]->getX() == x && actorVec[sublevel][i]->getY() == y) {
			int col = actorVec[sublevel][i]->bulletCollision();
			if (col == 1) {
				return 1;
			}
			if (col == 0) {
				out = 0;
			}
		}
	}
	if (m_player->getX() == x && m_player->getY() == y) {
		return m_player->bulletCollision();
	}
	return out;
}

void StudentWorld::placeObject(Actor* p, int sublevel) {
	actorVec[sublevel].push_back(p);
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
	oss << setw(3) << ammo;
	oss << "  TimeLimit: ";
	oss << setw(4) << time;
	string out = oss.str();
	return out;
}

int StudentWorld::subLevelLoader() {
	for (int i = 0; i < MAX_SUB_LEVEL; i++) {
		ostringstream oss;
		oss << "level";
		oss.fill('0');
		oss << setw(2) << getLevel();
		if (i != 0) {
			oss << "_" << setw(1) << i;
		}
		oss << ".dat";
		string out = oss.str();
		int status = loadLevel(out, i);
		if (status == GWSTATUS_PLAYER_WON) {
			return GWSTATUS_PLAYER_WON;
		}
		else if (status == GWSTATUS_LEVEL_ERROR) {
			return GWSTATUS_LEVEL_ERROR;
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::setSubLevel(int sl) {
	for (size_t i = 0; i < playerVec.size(); i++) {
		if (playerVec[i]->sublevel() == sl) {
			m_player = playerVec[i];
		}
	}
	m_numSubLevel = sl;
}

void StudentWorld::endLevel() {
	m_levelEnd = true;
}

void StudentWorld::openExit() {
	if (OPEN_EXIT) {
		return;
	}
	Actor* p;
	for (size_t i = 0; i < actorVec.size(); i++) {
		for (size_t j = 0; j < actorVec[i].size(); j++) {
			if (actorVec[i][j]->getID() == IID_EXIT) {
				p = actorVec[i][j];
			}
			if (actorVec[i][j]->getID() == IID_GOLD || actorVec[i][j]->getID() == IID_HOSTAGE || actorVec[i][j]->getID() == IID_ROBOT_BOSS) {
				return;
			}
		}
	}
	playSound(SOUND_REVEAL_EXIT);
	p->activate(true);
	OPEN_EXIT = true;
}
int StudentWorld::getTick() {
	return m_time;
}
void StudentWorld::equalizeStats() {
	int h = m_player->health();
	int a = m_player->ammo();
	for (size_t i = 0; i < playerVec.size(); i++) {
		playerVec[i]->setHealth(h);
		playerVec[i]->setAmmo(a);
	}
}

bool StudentWorld::checkGrid(int dim, int ID, int sublevel, Actor* center, Actor*& output) {
	int centerX = center->getX();
	int centerY = center->getY();
	for (size_t i = 0; i < actorVec[sublevel].size(); i++) {
		if ((actorVec[sublevel][i]->getX() >= centerX - dim / 2 && actorVec[sublevel][i]->getX() <= centerX + dim / 2) &&
			(actorVec[sublevel][i]->getY() >= centerY - dim / 2 && actorVec[sublevel][i]->getY() <= centerY + dim / 2)) {
			if (actorVec[sublevel][i]->getID() == ID && actorVec[sublevel][i]->alive()) {
				output = actorVec[sublevel][i];
				return true;
			}
		}
	}
	return false;
}
int StudentWorld::countGrid(int dim, int ID, int sublevel, Actor* center) {
	int centerX = center->getX();
	int centerY = center->getY();
	int count = 0;
	for (size_t i = 0; i < actorVec[sublevel].size(); i++) {
		if ((actorVec[sublevel][i]->getX() >= centerX - dim / 2 && actorVec[sublevel][i]->getX() <= centerX + dim / 2) &&
			(actorVec[sublevel][i]->getY() >= centerY - dim / 2 && actorVec[sublevel][i]->getX() <= centerY + dim / 2)) {
			if (actorVec[sublevel][i]->getID() == ID && actorVec[sublevel][i]->alive()) {
				count++;
			}
		}
	}
	return count;
}
Player* StudentWorld::player() {
	return m_player;
}
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
