#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, int sublevel, Direction startDirection, StudentWorld* world);
	virtual void doSomething() = 0;				// dictates what the actor does during a tick
	virtual int bulletCollision() = 0;			// return 0 if object blocks bullets, returns 1 if object takes damage, returns 2 if bullet goes through
	virtual bool passThrough() = 0;
	bool alive();
	virtual void setDead();
	int sublevel();
	void moveSubLevel(int in);
	void activate(bool on);
	bool active();
	void kill();
	StudentWorld* world();
private:
	StudentWorld* m_world;
	bool ACTIVE;
	int m_sublevel;
	bool m_alive;
};
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class ImmobileObject : public Actor {
public:	
	ImmobileObject(int imageID, int startX, int startY, int sublevel, StudentWorld* world);
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

class Waterpool : public SolidObject {
public:
	Waterpool(int startX, int startY, int sublevel);
	virtual void doSomething();
private:
	int m_health;
};

class Nest : public SolidObject {
public:
	Nest(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void doSomething();
private:
	StudentWorld* m_world;
};

class Item : public ImmobileObject {
public:
	Item(int imageID, int startX, int startY, StudentWorld* world, int sublevel);
	virtual void doSomething();
	virtual void itemPickUp() = 0;
private:
};
class Gate : public Item {
public:
	Gate(int startX, int startY, StudentWorld* world, int sublevel, int destination);
	virtual void itemPickUp();
private:
	int m_dest;
};
class Exit :public Item {
public:
	Exit(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void itemPickUp();
};
class Hostage : public Item {
public:
	Hostage(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void itemPickUp();
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
class Farplane :public Item {
public:
	Farplane(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void itemPickUp();
};

class MobileObject : public Actor {
public:
	MobileObject(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel);
	virtual bool passThrough();
	virtual int bulletCollision();
	void fire();
	int health();
	void setHealth(int newh);
private:
	int m_health;
};

class Enemy : public MobileObject {
public:
	Enemy(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel);
	bool doIRest();
	bool eyeSight();
	bool move();
private:
	int m_tick;
	int m_moveRate;
};

class Gangster : public Enemy {
public:
	Gangster(int startX, int startY, int startDirection, StudentWorld* world, int sublevel, int health = 10, int imageID = IID_GANGSTER);
	virtual void doSomething();
	virtual void setDead();
};

class Robot : public Gangster {
public:
	Robot(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void setDead();
};

class Bully : public Enemy {
public:
	Bully(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void doSomething();
	virtual void setDead();
private:
	int TURN_DISTANCE;
	int m_goodies[3];
};

class Player : public MobileObject{
public:
	Player(int startX, int startY, StudentWorld* world, int sublevel);
	virtual void doSomething();
	int ammo();
	void increaseAmmo(int in);
	void damage(int in);
	void setAmmo(int in);
private:
	int ammunition;
	int gateX;
	int gateY;
};

class Bullet : public Actor {
public:
	Bullet(int startX, int startY, Direction startDirection, StudentWorld* world, int sublevel);
	virtual void doSomething();
	virtual int bulletCollision();
	virtual bool passThrough();
private:
};

#endif // ACTOR_H_
