#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, Direction startDirection);
	virtual void doSomething() = 0;				// dictates what the actor does during a tick
	virtual int bulletCollision() = 0;			// return 0 if object blocks bullets, returns 1 if object takes damage, returns 2 if bullet goes through
	virtual bool passThrough() = 0;
	bool alive();
	void setDead();
private:
	bool m_alive;
};
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class ImmobileObject : public Actor {
public:	
	ImmobileObject(int imageID, int startX, int startY);
	virtual void doSomething();				// dictates what the actor does during a tick
	virtual int bulletCollision();
	virtual bool passThrough();
};

class SolidObject : public ImmobileObject {
public:
	SolidObject(int imageID, int startX, int startY);
	virtual int bulletCollision();
	virtual bool passThrough();
};

class Wall : public SolidObject {
public:
	Wall(int startX, int startY);
};

class FakeWall : public ImmobileObject {
public:
	FakeWall(int startX, int startY);
};

class StudentWorld;
class Item : public ImmobileObject {
public:
	Item(int imageID, int startX, int startY, StudentWorld* world);
	StudentWorld* world();
	virtual void doSomething();
	virtual void itemPickUp() = 0;
private:
	StudentWorld* m_world;
};

class Gold : public Item {
public:
	Gold(int startX, int startY, StudentWorld* world);
	virtual void itemPickUp();
};
class RestoreHealth : public Item {
public:
	RestoreHealth(int startX, int startY, StudentWorld* world);
	virtual void itemPickUp();
};
class Ammo : public Item {
public:
	Ammo(int startX, int startY, StudentWorld* world);
	virtual void itemPickUp();
};
class ExtraLife : public Item {
public:
	ExtraLife(int startX, int startY, StudentWorld* world);
	virtual void itemPickUp();
};
class MobileObject : public Actor {
public:
	MobileObject(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world);
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
	Player(int startX, int startY, int subLevel, StudentWorld* world);
	virtual void doSomething();
	int ammo();
	void increaseAmmo(int in);
private:
	int ammunition;
	int subLevel;
};

class Bullet : public Actor {
public:
	Bullet(int startX, int startY, Direction startDirection, StudentWorld* world);
	virtual void doSomething();
	virtual int bulletCollision();
	virtual bool passThrough();
private:
	StudentWorld* m_world;
};

#endif // ACTOR_H_
