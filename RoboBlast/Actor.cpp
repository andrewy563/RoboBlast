#include "Actor.h"
#include "StudentWorld.h"
#include <cstdlib>												// for rand()
#include <ctime>												// for srand()
#include <iostream>												// for cerr

//////////////////////////
// Actor Implementation //
//////////////////////////

Actor::Actor(int imageID, int startX, int startY, int sublevel, Direction startDirection = none, StudentWorld* world = nullptr) :
	GraphObject(imageID, startX, startY, startDirection, sublevel) {
	m_alive = true;
	m_sublevel = sublevel;
	setVisible(true);
	m_world = world;
}

Actor::~Actor() {};

bool Actor::alive() {
	return m_alive;
}

void Actor::setDead() {
	m_alive = false;
	setVisible(false);
}

int Actor::sublevel() {
	return m_sublevel;
}

void Actor::activate(bool on) {
	ACTIVE = on;
	setVisible(on);
}

bool Actor::active() {
	return ACTIVE;
}

StudentWorld* Actor::world() {
	return m_world;
}

void Actor::kill() {
	m_alive = false;
}

///////////////////////////////////
// ImmobileObject Implementation //
///////////////////////////////////

ImmobileObject::ImmobileObject(int imageID, int startX, int startY, int sublevel, StudentWorld* world = nullptr) 
	: Actor(imageID, startX, startY, sublevel, none, world) {}

ImmobileObject::~ImmobileObject() {}

void ImmobileObject::doSomething() {}		// immobile objects generally don't do anything

int ImmobileObject::bulletCollision() {
	return 2;
}

bool ImmobileObject::passThrough() {
	return true;
}

/////////////////////////////
// FakeWall Implementation //
/////////////////////////////

FakeWall::FakeWall(int startX, int startY, int sublevel) : ImmobileObject(IID_FAKE_WALL, startX, startY, sublevel) {}

FakeWall::~FakeWall() {
	std::cerr << "Destructing FakeWall" << std::endl;
}

////////////////////////////////
// SolidObject Implementation //
////////////////////////////////

SolidObject::SolidObject(int imageID, int startX, int startY, int sublevel) : ImmobileObject(imageID, startX, startY, sublevel) {}

SolidObject::~SolidObject() {}

int SolidObject::bulletCollision() {
	return 0;
}

bool SolidObject::passThrough() {
	return false;
}

/////////////////////////
// Wall Implementation //
/////////////////////////

Wall::Wall(int startX, int startY, int sublevel) : SolidObject(IID_WALL, startX, startY, sublevel) {}

Wall::~Wall(){
	std::cerr << "Destructing Wall" << std::endl;
}

//////////////////////////////
// Waterpool Implementation //
//////////////////////////////

Waterpool::Waterpool(int startX, int startY, int sublevel) : SolidObject(IID_WATERPOOL, startX, startY, sublevel) {
	m_health = 30;
}

Waterpool::~Waterpool() {
	std::cerr << "Destructing Waterpool" << std::endl;
}

void Waterpool::doSomething() {
	if (!alive())
		return;

	m_health -= 1;							// whirlpool should exists for only 30 ticks
	if (m_health <= 0) {
		setDead();
		return;
	}
}

/////////////////////////
// Nest Implementation //
/////////////////////////

Nest::Nest(int startX, int startY, StudentWorld* world, int sublevel) : SolidObject(IID_BULLY_NEST, startX, startY, sublevel) {
	m_world = world;
}

Nest::~Nest() {
	std::cerr << "Destructing Nest" << std::endl;
}

void Nest::doSomething() {
	srand(time(NULL));

	int num = (rand()) % 50 + 1;			// random number between 1 and 50 that is used for the 1 in 50 chance

	if (m_world->countGrid(7, IID_BULLY, sublevel(), this) < 3 && m_world->countGrid(1, IID_BULLY, sublevel(), this) == 0) {
		int chance = ((rand() * m_world->getTick()) % 50) + 1;	// a number is chosen from 1 to 50 and is compared to the first number chosen

		if (chance == num) {									// if the two random numbers are the same, then create a new Bully object
			Bully * b = new Bully(getX(), getY(), m_world, sublevel());
			m_world->placeObject(b, sublevel());
			m_world->playSound(SOUND_BULLY_BORN);
		}

	}
}

/////////////////////////
// Item Implementation //
/////////////////////////

Item::Item(int imageID, int startX, int startY, StudentWorld* world, int sublevel) 
	: ImmobileObject(imageID, startX, startY, sublevel, world) {
}

Item::~Item() {}

void Item::doSomething() {
	if (!alive()) {
		return;
	}

	if (world()->player()->getX() == getX() && world()->player()->getY() == getY() && world()->player()->sublevel() == sublevel() && active()) {
		itemPickUp();											// different depending on the type of item
		setDead();
	}
}

/////////////////////////
// Gate Implementation //
/////////////////////////

Gate::Gate(int startX, int startY, StudentWorld* world, int sublevel, int destination) : Item(IID_GATE, startX, startY, world, sublevel) {
	m_dest = destination;
}

Gate::~Gate() {
	std::cerr << "Destructing Gate" << std::endl;
}

void Gate::itemPickUp() {
	world()->setSubLevel(m_dest);								// tells the world to move sublevels
}

/////////////////////////
// Exit Implementation //
/////////////////////////

Exit::Exit(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_EXIT, startX, startY, world, sublevel) {
	activate(false);
}

Exit::~Exit() {
	std::cerr << "Destructing Exit" << std::endl;
}

void Exit::itemPickUp() {
	world()->increaseScore(1500);
	world()->endLevel();
}

////////////////////////////
// Hostage Implementation //
////////////////////////////

Hostage::Hostage(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_HOSTAGE, startX, startY, world, sublevel) {}

Hostage::~Hostage() {
	std::cerr << "Destructing Wall" << std::endl;
}
void Hostage::itemPickUp() {
	world()->playSound(SOUND_GOT_GOODIE);
}

/////////////////////////
// Gold Implementation //
/////////////////////////

Gold::Gold(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_GOLD, startX, startY, world, sublevel) {}

Gold::~Gold() {
	std::cerr << "Destructing Gold" << std::endl;
}

void Gold::itemPickUp() {
	world()->increaseScore(100);
	world()->playSound(SOUND_GOT_GOLD);
}

//////////////////////////////////
// RestoreHealth Implementation //
//////////////////////////////////

RestoreHealth::RestoreHealth(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_RESTORE_HEALTH, startX, startY, world, sublevel) {}

RestoreHealth::~RestoreHealth() {
	std::cerr << "Destructing RestoreHealth" << std::endl;
}

void RestoreHealth::itemPickUp() {
	world()->increaseScore(100);
	world()->player()->setHealth(20);
	world()->playSound(SOUND_GOT_GOODIE);
}

/////////////////////////
// Ammo Implementation //
/////////////////////////

Ammo::Ammo(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_AMMO, startX, startY, world, sublevel) {}

Ammo::~Ammo() {
	std::cerr << "Destructing Ammo" << std::endl;
}

void Ammo::itemPickUp() {
	world()->increaseScore(150);
	world()->player()->setAmmo(world()->player()->ammo() + 25);
	world()->playSound(SOUND_GOT_GOODIE);
}

//////////////////////////////
// ExtraLife Implementation //
//////////////////////////////

ExtraLife::ExtraLife(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_EXTRA_LIFE, startX, startY, world, sublevel) {}

ExtraLife::~ExtraLife() {
	std::cerr << "Destructing ExtraLife" << std::endl;
}

void ExtraLife::itemPickUp() {
	world()->increaseScore(150);
	world()->incLives();
	world()->playSound(SOUND_GOT_GOODIE);
}

/////////////////////////////
// Farplane Implementation //
/////////////////////////////

Farplane::Farplane(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_FARPLANE_GUN, startX, startY, world, sublevel) {}

Farplane::~Farplane() {
	std::cerr << "Destructing Farplane" << std::endl;
}

void Farplane::itemPickUp() {
	world()->player()->setHealth(world()->player()->health() - 10);							// decrease player health

	if (world()->player()->health() <= 0) {
		world()->player()->setDead();
		world()->playSound(SOUND_PLAYER_DIE);
	}

	Actor* p;
	world()->playSound(SOUND_GOT_FARPLANE_GUN);
	while (world()->checkGrid(9, IID_GANGSTER, sublevel(), this, p)) {						// checks the world() for gangsters and bullies
		p->setDead();
	}
	while (world()->checkGrid(9, IID_BULLY, sublevel(), this, p)) {
		p->setDead();
	}
}



/////////////////////////////////
// MobileObject Implementation //
/////////////////////////////////

MobileObject::MobileObject(int imageID, int startX, int startY, Direction startDirection, StudentWorld* world, int sublevel)
	: Actor(imageID, startX, startY, sublevel, startDirection, world) {}

MobileObject::~MobileObject() {}

bool MobileObject::move() {
	// Move the object in its direction. Return false if object cannot move
	switch (getDirection()) {

	case left:
		if (world()->canIPass(getX() - 1, getY(), sublevel())) {
			moveTo(getX() - 1, getY());
			return true;
		}
		break;

	case right:
		if (world()->canIPass(getX() + 1, getY(), sublevel())) {
			moveTo(getX() + 1, getY());
			return true;
		}
		break;

	case up:
		if (world()->canIPass(getX(), getY() + 1, sublevel())) {
			moveTo(getX(), getY() + 1);
			return true;
		}
		break;

	case down:
		if (world()->canIPass(getX(), getY() - 1, sublevel())) {
			moveTo(getX(), getY() - 1);
			return true;
		}
		break;
	}

	return false;
}

///////////////////////////
// Bullet Implementation //
////////////.//////////////

Bullet::Bullet(int startX, int startY, Direction startDir, StudentWorld* world, int sublevel)
	: MobileObject(IID_BULLET, startX, startY, startDir, world, sublevel) {}

Bullet::~Bullet() {
	std::cerr << "Destructing Bullet" << std::endl;
}

void Bullet::doSomething() {
	if (!alive()) {
		return;
	}

	int check = 2;

	check = world()->bulletColCheck(getX(), getY(), sublevel());	// check colision in the square that bullet is on

	if (check == 1 || check == 0) {					// if check = 1 or 2, then the bullet collided with something
		setDead();
		return;
	}

	if (!move()) {									// if the bullet cannot move, then check for bullet collision
		switch (getDirection()) {

		case left:
			check = world()->bulletColCheck(getX() - 1, getY(), sublevel());
			break;

		case right:
			check = world()->bulletColCheck(getX() + 1, getY(), sublevel());
			break;

		case up:
			check = world()->bulletColCheck(getX(), getY() + 1, sublevel());
			break;

		case down:
			check = world()->bulletColCheck(getX(), getY() - 1, sublevel());
			break;
		}
	}

	if (check == 1 || check == 0) {
		setDead();
		return;
	}
}

int Bullet::bulletCollision() {
	return 2;
}

bool Bullet::passThrough() {
	return true;
}

//////////////////////////////
// Character Implementation //
//////////////////////////////

Character::Character(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel)
	: MobileObject(imageID, startX, startY, startDirection, world, sublevel) {
	m_health = health;
}

Character::~Character() {}

bool Character::passThrough() {
	return false;
}

int Character::bulletCollision() {							// if a bullet collides a MobileObject, it should damage the MobileObject
	m_health -= 2;
	if (m_health <= 0) {										// if a player dies to a bullet, set the player to dead and play the death sound
		setDead();
		if (getID() == IID_PLAYER) {
			world()->playSound(SOUND_PLAYER_DIE);
		}
		return 1;
	}

	if (getID() == IID_PLAYER) {								// play a sound on impact depending on who gets hit
		world()->playSound(SOUND_PLAYER_IMPACT);
	}
	else {
		world()->playSound(SOUND_ENEMY_IMPACT);
	}

	return 1;
}

int Character::health() {
	return m_health;
}

void Character::fire() {									// creates a bullet in the square adjacent to the object in the direction the object
															// is facing and adds it to the StudentWorld
	Bullet* b;
	switch (getDirection()) {
	case left:
		b = new Bullet(getX() - 1, getY(), left, world(), sublevel());
		break;
	case right:
		b = new Bullet(getX() + 1, getY(), right, world(), sublevel());
		break;
	case up:
		b = new Bullet(getX(), getY() + 1, up, world(), sublevel());
		break;
	case down:
		b = new Bullet(getX(), getY() - 1, down, world(), sublevel());
		break;
	}
	world()->placeObject(b, sublevel());
}

void Character::setHealth(int newh) {
	m_health = newh;
}

//////////////////////////
// Enemy Implementation //
//////////////////////////

Enemy::Enemy(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel)
	: Character(imageID, startX, startY, startDirection, health, world, sublevel) {
	m_moveRate = (28 - this->world()->getLevel()) / 4;					// set the moverate according to the formula provided in the spec
	m_tick = 0;
	if (m_moveRate < 3)
		m_moveRate = 3;
}

Enemy::~Enemy() {}

bool Enemy::doIRest() {													// determines whether or not enemy should act on a given tick
	m_tick += 1;
	if (m_tick % m_moveRate == 0) {
		return false;
	}
	return true;
}

bool Enemy::eyeSight() {												

	// Checks if player is in sight by seeing if player is in the direction that the enemy is facing.
	// Stops checking if the point being examined is outside the level or if it encounters an object that cannot be passed, such as a Wall

	int x = getX();
	int y = getY();

	switch (getDirection()) {

	case right:
		while (x < VIEW_WIDTH) {
			x += 1;
			if (world()->player()->getX() == x && world()->player()->getY() == y) {
				return true;
			}
			if (!world()->canIPass(x, y, sublevel())) {
				return false;
			}
		}
		break;

	case left:
		while (x > 0) {
			x -= 1;
			if (world()->player()->getX() == x && world()->player()->getY() == y)
				return true;
			if (!world()->canIPass(x, y, sublevel())) {
				return false;
			}

		}
		break;	

	case up:
			while (y < VIEW_WIDTH) {
				y += 1;
				if (world()->player()->getX() == x && world()->player()->getY() == y)
					return true;
				if (!world()->canIPass(x, y, sublevel())) {
					return false;
				}
			}
			break;

	case down:
		while (y > 0) {
			y -= 1;
			if (world()->player()->getX() == x && world()->player()->getY() == y)
				return true;
			if (!world()->canIPass(x, y, sublevel())) {
				return false;
			}
		}
		break;
	}

	return false;
}

void Enemy::setDead() {
	setVisible(false);
	kill();
	drop();
	world()->playSound(SOUND_ENEMY_DIE);
}

/////////////////////////////
// Gangster Implementation //
/////////////////////////////

Gangster::Gangster(int startX, int startY, int startDirection, StudentWorld* world, int sublevel, int health, int imageID) 
	: Enemy(imageID, startX, startY, right, health, world, sublevel) {

	if (startDirection == 2) {								// instead of creating separate horizontal and vertical gangster classes, 
		setDirection(down);
	}
}

Gangster::~Gangster() {
	if (getID() == IID_GANGSTER)
		std::cerr << "Destructing Gangster" << std::endl;
}

void Gangster::doSomething() {

	if (!alive() || doIRest()) {
		return;
	}

	if (eyeSight()) {										// if the player is in the Gangster's sight, gangster should fire a bullet
		world()->playSound(SOUND_ENEMY_FIRE);
		fire();
		return;
	}

	if (!move()) {											// Move the gangster, and if it cannot move, switch the direction 
		switch (getDirection()) {
		case left:
			setDirection(right);
			break;
		case right:
			setDirection(left);
			break;
		case up:
			setDirection(down);
			break;
		case down:
			setDirection(up);
			break;
		}
	}
}

void Gangster::drop() {
	Waterpool* w = new Waterpool(getX(), getY(), sublevel());					// create a Waterpool at the Gangster's location
	world()->placeObject(w, sublevel());
}

//////////////////////////
// Robot Implementation //
//////////////////////////

Robot::Robot(int startX, int startY, StudentWorld* world, int sublevel)			// Robot Boss is essentially a horizontal gangster with better hp
	: Gangster(startX, startY, 1, world, sublevel, 50, IID_ROBOT_BOSS) {}		// and it drops a Hostage instead of a Waterpool

Robot::~Robot() {
	std::cerr << "Destructing Robot" << std::endl;
}

void Robot::drop() {
	Hostage* h = new Hostage(getX(), getY(), world(), sublevel());				// create a hostage at the Robot's location
	world()->placeObject(h, sublevel());
}

//////////////////////////
// Bully Implementation //
//////////////////////////

Bully::Bully(int startX, int startY, StudentWorld* world, int sublevel)
	: Enemy(IID_BULLY, startX, startY, right, 5, world, sublevel) {
	m_goodies[0] = 0;															// number of RestoreHealth picked up
	m_goodies[1] = 0;															// number of Ammo picked up
	m_goodies[2] = 0;															// number of Extra life picked up
	srand(time(NULL));
	TURN_DISTANCE = (((rand()) * this->world()->getTick()) % 6) + 1;			// picks a random number from 1 to 6 inclusive
}

Bully::~Bully() {
	std::cerr << "Destructing Bully" << std::endl;
}

void Bully::doSomething() {

	if (!alive())
		return;

	if (doIRest()) {
		return;
	}

	if (eyeSight()) {
		world()->playSound(SOUND_ENEMY_FIRE);
		fire();
		return;
	}

	Actor* p;

	if (world()->checkGrid(1, IID_RESTORE_HEALTH, sublevel(), this, p) ||			// checks if Bully is on the same square as a goodie
		world()->checkGrid(1, IID_AMMO, sublevel(), this, p) ||						// destroys the goodie object and adds 1 to the respect counter
		world()->checkGrid(1, IID_EXTRA_LIFE, sublevel(), this, p)) {

		if (p->getID() == IID_RESTORE_HEALTH) {
			m_goodies[0] += 1;
		}

		else if (p->getID() == IID_AMMO) {
			m_goodies[1] += 1;
		}

		else if (p->getID() == IID_EXTRA_LIFE) {
			m_goodies[2] += 1;
		}
		world()->playSound(SOUND_BULLY_MUNCH);
		p->setDead();
		return;
	}

	if (TURN_DISTANCE != 0) {
		if (!move())														// if Bully can not move, set TURN_DISTANCE to 0 so it can change
			TURN_DISTANCE = 0;												// directions
		else {
			TURN_DISTANCE -= 1;
			return;
		}
	}

	if (TURN_DISTANCE == 0) {
		srand(time(NULL));
		TURN_DISTANCE = (((rand()) * this->world()->getTick()) % 6) + 1;	// picks a random number from 1 to 6 inclusive
		int d = rand() % 4;													// picks a random direction (from 0 to 3)
		Direction list[4] = { left, right, up, down };
		int count = 0;
		while (count < 4) {													// attempts to find a direction in which bully can move
			setDirection(list[(d + count) % 4]);			
			if (move()) {
				return;
			}
			count++;
		}
	}
}

void Bully::drop() {
// create new goodie objects where Bully died
	for (int i = 0; i < m_goodies[0]; i++) {
		RestoreHealth* p = new RestoreHealth(getX(), getY(), world(), sublevel());
		world()->placeObject(p, sublevel());
	}
	for (int i = 0; i < m_goodies[1]; i++) {
		Ammo* p = new Ammo(getX(), getY(), world(), sublevel());
		world()->placeObject(p, sublevel());
	}
	for (int i = 0; i < m_goodies[2]; i++) {
		ExtraLife* p = new ExtraLife(getX(), getY(), world(), sublevel());
		world()->placeObject(p, sublevel());
	}
}

///////////////////////////
// Player Implementation //
///////////////////////////

Player::Player(int startX, int startY, StudentWorld* world, int sublevel) : Character(IID_PLAYER, startX, startY, right, 20, world, sublevel) {
	ammunition = 0; 
}

Player::~Player() {
	std::cerr << "Destructing Player" << std::endl;
}

void Player::doSomething() {				// player can kill themself, fire, or move depending on the keypress
	int ch;
	if (world()->getKey(ch)) {
		switch (ch) {
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
		{
			if (ammunition > 0) {
				ammunition -= 1;
				world()->playSound(SOUND_PLAYER_FIRE);
				fire();
			}
			break; 
		}		
		case KEY_PRESS_LEFT:
		{
			setDirection(left);
			move();
			break;
		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(right);
			move();
			break;
		}
		case KEY_PRESS_UP:
		{
			setDirection(up);
			move();
			break;
		}
		case KEY_PRESS_DOWN:
		{
			setDirection(down);
			move();
			break;
		}
		}
	}
}

int Player::ammo() {
	return ammunition;
}

void Player::setAmmo(int n) {
	ammunition = n;
}