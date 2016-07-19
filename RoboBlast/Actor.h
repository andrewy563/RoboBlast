#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class Actor: public GraphObject {
public:
	Actor(int imageID, int startX, int startY, Direction startDirection);
	virtual void doSomething() = 0;				// dictates what the actor does during a tick
	virtual int bulletCollision() = 0;
	virtual bool passThrough() = 0;
private:
};
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class immobileObject : public Actor {
public:	
	immobileObject(int imageID, int startX, int startY);
	virtual void doSomething();				// dictates what the actor does during a tick
};

class solidObject : public immobileObject {
public:
	solidObject(int imageID, int startX, int startY);
	virtual int bulletCollision();
	virtual bool passThrough();
};

class Wall : public solidObject {
public:
	Wall(int startX, int startY);
};

class mobileObject : public Actor {
public:
	mobileObject(int imageID, int startX, int startY, Direction startDirection, int health);
	virtual int damage(int d);
	virtual bool passThrough();
	virtual bool alive();
	virtual void setDead();
	virtual int bulletCollision();
private:
	bool ALIVE;
	int m_health;
};

class StudentWorld;
class Player : public mobileObject{
public:
	Player(int startX, int startY, int subLevel, StudentWorld* world);
	virtual void doSomething();
	virtual int bulletCollision();
private:
	int ammunition;
	int subLevel;
	StudentWorld* m_world;
};
#endif // ACTOR_H_
