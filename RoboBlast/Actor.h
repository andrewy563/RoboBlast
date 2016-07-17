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

class mobileObject : public immobileObject {
public:
	virtual int bulletCollision();
	virtual bool passThrough();
private:
	int health;
	
};

class Player : public mobileObject{
public:
	Player(int imageID, int startX, int startY);
	virtual void doSomething();
};
#endif // ACTOR_H_
