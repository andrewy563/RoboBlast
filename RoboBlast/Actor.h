#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, int sublevel, Direction startDirection, StudentWorld* world);
	virtual ~Actor();							// destructor, will print out "Destroying 'type of object'" to cerr
	virtual void doSomething() = 0;				// dictates what the actor does during a tick
	virtual int bulletCollision() = 0;			// return 0 if object blocks bullets, returns 1 if object takes damage, returns 2 if bullet goes through
	virtual bool passThrough() = 0;				// return true if things can pass through the Actor
	bool alive();								// returns true if object is alive
	virtual void setDead();						// sets the object to dead, marks it for deletion
	int sublevel();								// returns what sublevel the object is on
	void activate(bool on);						// used by StudentWorld in order to activate an Exit
	bool active();								// checks if Exit is active
	void kill();								// sets the object to dead, used for redefining setDead() in derived classes.
	StudentWorld* world();						// returns a pointer to the Student World
private:
	StudentWorld* m_world;
	bool ACTIVE;
	int m_sublevel;
	bool m_alive;
};

/////////////////////
// Immobile object //
/////////////////////

class ImmobileObject : public Actor {
public:	
	ImmobileObject(int imageID, int startX, int startY, int sublevel, StudentWorld* world);
	virtual ~ImmobileObject();
	virtual void doSomething();				// dictates what the actor does during a tick
	virtual int bulletCollision();
	virtual bool passThrough();
};

///////////////
// Fake wall //
///////////////

class FakeWall : public ImmobileObject {
public:
	FakeWall(int startX, int startY, int sublevel);
	virtual ~FakeWall();
};

//////////////////////////////
// Impassable Actor Classes //
//////////////////////////////

class SolidObject : public ImmobileObject {
public:
	SolidObject(int imageID, int startX, int startY, int sublevel);
	virtual ~SolidObject();
	virtual int bulletCollision();
	virtual bool passThrough();
};

class Wall : public SolidObject {
public:
	Wall(int startX, int startY, int sublevel);
	virtual ~Wall();
};

class Waterpool : public SolidObject {
public:
	Waterpool(int startX, int startY, int sublevel);
	virtual ~Waterpool();
	virtual void doSomething();									// decreases m_health by 1 per tick
private:
	int m_health;
};

class Nest : public SolidObject {
public:
	Nest(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Nest();
	virtual void doSomething();									// spawns a Bully if conditions are met
private:
	StudentWorld* m_world;
};

///////////////////////////////
// Pickupable  Actor Classes //
///////////////////////////////

class Item : public ImmobileObject {
public:
	Item(int imageID, int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Item();
	virtual void doSomething();									// checks if player stands on item and calls itemPickUp();
	virtual void itemPickUp() = 0;								// dictates what happens when the Item is picked up
private:
};

class Gate : public Item {
public:
	Gate(int startX, int startY, StudentWorld* world, int sublevel, int destination);
	virtual ~Gate();
	virtual void itemPickUp();
private:
	int m_dest;													// gate's destination
};

class Exit :public Item {
public:
	Exit(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Exit();
	virtual void itemPickUp();
};

class Hostage : public Item {
public:
	Hostage(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Hostage();
	virtual void itemPickUp();
};

class Gold : public Item {
public:
	Gold(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Gold();
	virtual void itemPickUp();
};

class RestoreHealth : public Item {
public:
	RestoreHealth(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~RestoreHealth();
	virtual void itemPickUp();
};

class Ammo : public Item {
public:
	Ammo(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Ammo();
	virtual void itemPickUp();
};

class ExtraLife : public Item {
public:
	ExtraLife(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~ExtraLife();
	virtual void itemPickUp();
};

class Farplane :public Item {
public:
	Farplane(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Farplane();
	virtual void itemPickUp();
};

//////////////////////////
// Moving Actor Classes //
//////////////////////////

class MobileObject : public Actor {								// all moveable objects
public:
	MobileObject(int imageID, int startX, int startY, Direction startDirection, StudentWorld* world, int sublevel);
	virtual ~MobileObject();
	bool move();												// moves the object unless there is an obstruction
};

class Bullet : public MobileObject {
public:
	Bullet(int startX, int startY, Direction startDirection, StudentWorld* world, int sublevel);
	virtual ~Bullet();
	virtual void doSomething();
	virtual int bulletCollision();
	virtual bool passThrough();
};

class Character : public MobileObject {								// players and enemies							
public:
	Character(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel);
	virtual ~Character();
	virtual bool passThrough();
	virtual int bulletCollision();
	void fire();												// function that handles firing of bullets
	int health();												// returns the current health that the actor has
	void setHealth(int newh);									// sets the health variable to newh
private:
	int m_health;
};

class Enemy : public Character {
public:
	Enemy(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel);
	virtual ~Enemy();
	bool doIRest();												// checks whether the enemy should rest or not
	bool eyeSight();											// checks whether the player is in the enemy's line of sight
	virtual void setDead();										// plays dead sound and drops something based on what type of Enemy.
	virtual void drop() = 0;									// determines what the Enemy drops
private:
	int m_tick;													// number of ticks that the enemy is in the StudentWorld
	int m_moveRate;												// the rate at which the enemy moves
};

class Gangster : public Enemy {
public:
	Gangster(int startX, int startY, int startDirection, StudentWorld* world, int sublevel, int health = 10, int imageID = IID_GANGSTER);
	virtual ~Gangster();
	virtual void doSomething();
	virtual void drop();										// creates a new waterpool
};

class Robot : public Gangster {
public:
	Robot(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Robot();
	virtual void drop();										// Creates a new hostage
};

class Bully : public Enemy {
public:
	Bully(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Bully();
	virtual void doSomething();
	virtual void drop();										// Creates new goodies based on how many Goodies Bully picked up
private:
	int TURN_DISTANCE;											// distance that bully moves beore turning
	int m_goodies[3];											// keeps track of which goodies the bully picked up
};

class Player : public Character {
public:
	Player(int startX, int startY, StudentWorld* world, int sublevel);
	virtual ~Player();
	virtual void doSomething();
	int ammo();													// returns the ammo that the player has
	void setAmmo(int in);										// sets the ammo to a certain amount
private:
	int ammunition;												// keeps track of amount of ammo has
};

#endif // ACTOR_H_