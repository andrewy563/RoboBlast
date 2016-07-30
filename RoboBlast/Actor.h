#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, int sublevel, Direction startDirection);
	virtual void doSomething() = 0;				// dictates what the actor does during a tick
	virtual int bulletCollision() = 0;			// return 0 if object blocks bullets, returns 1 if object takes damage, returns 2 if bullet goes through
	virtual bool passThrough() = 0;
	bool alive();
	void setDead();
	int sublevel();
	void moveSubLevel(int in);
private:
	int m_sublevel;
	bool m_alive;
};
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class ImmobileObject : public Actor {
public:	
	ImmobileObject(int imageID, int startX, int startY, int sublevel);
	virtual void doSomething();				// dictates what the actor does during a tick
	virtual int bulletCollision();
	virtual bool passThrough();
};

class SolidObject : public ImmobileObject {
public:
	SolidObject(int imageID, int startX, int startY, int sublevel);
	virtual int bulletCollision();
	virtual bool passThrough();
};

class Wall : public SolidObject {
public:
	Wall(int startX, int startY, int sublevel);
};

class FakeWall : public ImmobileObject {
public:
	FakeWall(int startX, int startY, int sublevel);
};

class StudentWorld;
class Item : public ImmobileObject {
public:
	Item(int imageID, int startX, int startY, StudentWorld* world, int sublevel);
	StudentWorld* world();
	virtual void doSomething();
	virtual void itemPickUp() = 0;
private:
	StudentWorld* m_world;
};
class Gate : public Item {
public:
	Gate(int startX, int startY, StudentWorld* world, int sublevel, int destination);
	virtual void itemPickUp();
private:
	int m_dest;
};
class Gold : public Item {
public:
	Gold(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void itemPickUp();
};
class RestoreHealth : public Item {
public:
	RestoreHealth(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void itemPickUp();
};
class Ammo : public Item {
public:
	Ammo(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void itemPickUp();
};
class ExtraLife : public Item {
public:
	ExtraLife(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void itemPickUp();
};
class MobileObject : public Actor {
public:
	MobileObject(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel);
	virtual bool passThrough();
	virtual int bulletCollision();
	virtual StudentWorld* world();
	void fire();
	int health();
	void setHealth(int newh);
private:
	int m_health;
	StudentWorld* m_world;
};

class Player : public MobileObject{
public:
	Player(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void doSomething();
	int ammo();
	void increaseAmmo(int in);
private:
	int ammunition;
};

class Bullet : public Actor {
public:
	Bullet(int startX, int startY, Direction startDirection, StudentWorld* world, int sublevel);
	virtual void doSomething();
	virtual int bulletCollision();
	virtual bool passThrough();
private:
	StudentWorld* m_world;
};

#endif // ACTOR_H_
